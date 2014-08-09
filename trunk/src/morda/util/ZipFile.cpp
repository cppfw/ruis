#include "ZipFile.hpp"

#include "unzip/unzip.h"

#include <sstream>
#include <ting/fs/FSFile.hpp>


using namespace morda;



namespace{

voidpf ZCALLBACK UnzipOpen(voidpf opaque, const char* filename, int mode){
	ting::fs::File* f = reinterpret_cast<ting::fs::File*>(opaque);
	
	switch(mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER){
		case ZLIB_FILEFUNC_MODE_READ:
			f->Open(ting::fs::File::READ);
			break;
		default:
			throw ting::fs::File::Exc("UnzipOpen(): tried opening zip file something else than READ. Only READ is supported.");
	}
	
	return f;
}

int ZCALLBACK UnzipClose(voidpf opaque, voidpf stream){
	ting::fs::File* f = reinterpret_cast<ting::fs::File*>(stream);
	f->Close();
	return 0;
}

uLong ZCALLBACK UnzipRead(voidpf opaque, voidpf stream, void* buf, uLong size){
	ting::fs::File* f = reinterpret_cast<ting::fs::File*>(stream);
	return uLong(f->Read(ting::ArrayAdaptor<std::uint8_t>(reinterpret_cast<std::uint8_t*>(buf), size)));
}

uLong ZCALLBACK UnzipWrite(voidpf opaque, voidpf stream, const void* buf, uLong size){
	ASSERT_INFO(false, "Writing ZIP files is not supported")
	return 0;
}

int ZCALLBACK UnzipError(voidpf opaque, voidpf stream){
	return 0;//no error
}

long ZCALLBACK UnzipSeek(voidpf  opaque, voidpf stream, uLong offset, int origin){
	ting::fs::File* f = reinterpret_cast<ting::fs::File*>(stream);
	
	//Assume that offset can only be positive, since its type is unsigned
	
	switch(origin){
		case ZLIB_FILEFUNC_SEEK_CUR:
			f->SeekForward(offset);
			return 0;
		case ZLIB_FILEFUNC_SEEK_END:
			f->SeekForward(size_t(-1));
			return 0;
		case ZLIB_FILEFUNC_SEEK_SET:
			f->Rewind();
			f->SeekForward(offset);
			return 0;
		default:
			return -1;
	}
}

long ZCALLBACK UnzipTell(voidpf opaque, voidpf stream){
	ting::fs::File* f = reinterpret_cast<ting::fs::File*>(stream);
	return long(f->CurPos());
}

}




ZipFile::ZipFile(std::unique_ptr<ting::fs::File> zipFile, const std::string& path) :
		ting::fs::File(path),
		zipFile(std::move(zipFile))
{
	zlib_filefunc_def ff;
	ff.opaque = this->zipFile.operator->();
	ff.zopen_file = &UnzipOpen;
	ff.zclose_file = &UnzipClose;
	ff.zread_file = &UnzipRead;
	ff.zwrite_file = &UnzipWrite;
	ff.zseek_file = &UnzipSeek;
	ff.zerror_file = &UnzipError;
	ff.ztell_file = &UnzipTell;
	
	this->handle = unzOpen2(this->zipFile->Path().c_str(), &ff);

	if(!this->handle){
		throw File::Exc("ZipFile::OpenZipFile(): opening zip file failed");
	}
}



ZipFile::~ZipFile()noexcept{
	this->Close();//make sure there is no file opened inside zip file

	if(unzClose(this->handle) != UNZ_OK){
		ASSERT(false)
	}
}



void ZipFile::OpenInternal(E_Mode mode) {
	if(mode != File::READ){
		throw File::Exc("illegal mode requested, only READ supported inside ZIP file");
	}

	if(unzLocateFile(this->handle, this->Path().c_str(), 0) != UNZ_OK){
		std::stringstream ss;
		ss << "ZipFile::OpenInternal(): file not found: " << this->Path();
		throw File::Exc(ss.str());
	}

	{
		unz_file_info zipFileInfo;

		if(unzGetCurrentFileInfo(this->handle, &zipFileInfo, 0, 0, 0, 0, 0, 0) != UNZ_OK){
			throw File::Exc("failed obtaining file info");
		}
	}

	if(unzOpenCurrentFile(this->handle) != UNZ_OK){
		throw File::Exc("file opening failed");
	}
}

void ZipFile::CloseInternal()noexcept{
	if(unzCloseCurrentFile(this->handle) == UNZ_CRCERROR){
		TRACE(<< "[WARNING] ZipFile::Close(): CRC is not good" << std::endl)
		ASSERT(false)
	}
}

size_t ZipFile::ReadInternal(ting::ArrayAdaptor<std::uint8_t> buf) {
	int numBytesRead = unzReadCurrentFile(this->handle, buf.begin(), buf.size());
	if(numBytesRead < 0){
		throw File::Exc("ZipFile::Read(): file reading failed");
	}

	ASSERT(numBytesRead >= 0)
	return size_t(numBytesRead);
}

bool ZipFile::Exists() const {
	//if directory
	if(this->Path()[this->Path().size() - 1] == '/'){
		return this->File::Exists();
	}
	
	if(this->Path().size() == 0){
		return false;
	}

	if(this->IsOpened()){
		return true;
	}
	
	return unzLocateFile(this->handle, this->Path().c_str(), 0) == UNZ_OK;
}



std::vector<std::string> ZipFile::ListDirContents(size_t maxEntries){
	if(!this->IsDir()){
		throw File::Exc("ZipFile::ListDirContents(): this is not a directory");
	}

	//if path refers to directory then there should be no files opened
	ASSERT(!this->IsOpened())

	if(!this->handle){
		throw File::Exc("ZipFile::ListDirContents(): zip file is not opened");
	}

	std::vector<std::string> files;

	//for every file, check if it is in the current directory
	{
		//move to first file
		int ret = unzGoToFirstFile(this->handle);

		if(ret != UNZ_OK){
			throw File::Exc("ZipFile::ListDirContents(): unzGoToFirstFile() failed.");
		}

		do{
			std::array<char, 255> fileNameBuf;

			if(unzGetCurrentFileInfo(
					this->handle,
					NULL,
					fileNameBuf.begin(),
					fileNameBuf.size(),
					NULL,
					0,
					NULL,
					0
				) != UNZ_OK)
			{
				throw File::Exc("ZipFile::ListDirContents(): unzGetCurrentFileInfo() failed.");
			}

			fileNameBuf[fileNameBuf.size() - 1] = 0;//null-terminate, just to be on a safe side

			std::string fn(fileNameBuf.begin());//filename

			if(fn.size() <= this->Path().size()){
				continue;
			}

			//check if full file path starts with the this->Path() string
			if(fn.compare(0, this->Path().size(), this->Path()) != 0){
				continue;
			}

			ASSERT(fn.size() > this->Path().size())
			std::string subfn(fn, this->Path().size(), fn.size() - this->Path().size());//subfilename

			size_t slashPos = subfn.find_first_of('/');

			//check if file is listed
			if(slashPos == std::string::npos){
				files.push_back(subfn);
				continue;
			}

			//if we get here then we need to add a directory
			ASSERT(subfn.size() >= slashPos + 1)
			files.push_back(
					std::string(subfn, 0, slashPos + 1)
				);

			if(files.size() == maxEntries){
				break;
			}
		}while((ret = unzGoToNextFile(this->handle)) == UNZ_OK);

		if(ret != UNZ_END_OF_LIST_OF_FILE && ret != UNZ_OK){
			throw File::Exc("ZipFile::ListDirContents(): unzGoToNextFile() failed.");
		}
	}

	std::vector<std::string> filesArray(files.size());
	for(unsigned i = 0; i < files.size(); ++i){
		filesArray[i] = files[i];
	}

	return filesArray;
}