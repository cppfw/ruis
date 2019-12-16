#include "ZipFile.hpp"

#include "unzip/unzip.h"

#include <sstream>
#include <papki/FSFile.hpp>


using namespace morda;



namespace{

voidpf ZCALLBACK UnzipOpen(voidpf opaque, const char* filename, int mode){
	papki::File* f = reinterpret_cast<papki::File*>(opaque);
	
	switch(mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER){
		case ZLIB_FILEFUNC_MODE_READ:
			f->open(papki::File::E_Mode::READ);
			break;
		default:
			throw std::invalid_argument("UnzipOpen(): tried opening zip file something else than READ. Only READ is supported.");
	}
	
	return f;
}

int ZCALLBACK UnzipClose(voidpf opaque, voidpf stream){
	papki::File* f = reinterpret_cast<papki::File*>(stream);
	f->close();
	return 0;
}

uLong ZCALLBACK UnzipRead(voidpf opaque, voidpf stream, void* buf, uLong size){
	papki::File* f = reinterpret_cast<papki::File*>(stream);
	return uLong(f->read(utki::Buf<std::uint8_t>(reinterpret_cast<std::uint8_t*>(buf), size)));
}

uLong ZCALLBACK UnzipWrite(voidpf opaque, voidpf stream, const void* buf, uLong size){
	ASSERT_INFO(false, "Writing ZIP files is not supported")
	return 0;
}

int ZCALLBACK UnzipError(voidpf opaque, voidpf stream){
	return 0;//no error
}

long ZCALLBACK UnzipSeek(voidpf  opaque, voidpf stream, uLong offset, int origin){
	papki::File* f = reinterpret_cast<papki::File*>(stream);
	
	//Assume that offset can only be positive, since its type is unsigned
	
	switch(origin){
		case ZLIB_FILEFUNC_SEEK_CUR:
			f->seekForward(offset);
			return 0;
		case ZLIB_FILEFUNC_SEEK_END:
			f->seekForward(size_t(-1));
			return 0;
		case ZLIB_FILEFUNC_SEEK_SET:
			f->rewind();
			f->seekForward(offset);
			return 0;
		default:
			return -1;
	}
}

long ZCALLBACK UnzipTell(voidpf opaque, voidpf stream){
	papki::File* f = reinterpret_cast<papki::File*>(stream);
	return long(f->curPos());
}

}




ZipFile::ZipFile(std::unique_ptr<papki::File> zipFile, const std::string& path) :
		papki::File(path),
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
	
	this->handle = unzOpen2(this->zipFile->path().c_str(), &ff);

	if(!this->handle){
		throw papki::exception("ZipFile::OpenZipFile(): opening zip file failed");
	}
}



ZipFile::~ZipFile()noexcept{
	this->close();//make sure there is no file opened inside zip file

	if(unzClose(this->handle) != UNZ_OK){
		ASSERT(false)
	}
}



void ZipFile::openInternal(E_Mode mode) {
	if(mode != File::E_Mode::READ){
		throw std::invalid_argument("illegal mode requested, only READ supported inside ZIP file");
	}

	if(unzLocateFile(this->handle, this->path().c_str(), 0) != UNZ_OK){
		std::stringstream ss;
		ss << "ZipFile::OpenInternal(): file not found: " << this->path();
		throw papki::exception(ss.str());
	}

	{
		unz_file_info zipFileInfo;

		if(unzGetCurrentFileInfo(this->handle, &zipFileInfo, 0, 0, 0, 0, 0, 0) != UNZ_OK){
			throw papki::exception("failed obtaining file info");
		}
	}

	if(unzOpenCurrentFile(this->handle) != UNZ_OK){
		throw papki::exception("file opening failed");
	}
}

void ZipFile::closeInternal()const noexcept{
	if(unzCloseCurrentFile(this->handle) == UNZ_CRCERROR){
		TRACE(<< "[WARNING] ZipFile::Close(): CRC is not good" << std::endl)
		ASSERT(false)
	}
}

size_t ZipFile::readInternal(utki::Buf<std::uint8_t> buf)const{
	ASSERT(buf.size() <= unsigned(-1))
	int numBytesRead = unzReadCurrentFile(this->handle, buf.begin(), unsigned(buf.size()));
	if(numBytesRead < 0){
		throw papki::exception("ZipFile::Read(): file reading failed");
	}

	ASSERT(numBytesRead >= 0)
	return size_t(numBytesRead);
}

bool ZipFile::exists()const{
	//if directory
	if(this->path()[this->path().size() - 1] == '/'){
		return this->File::exists();
	}
	
	if(this->path().size() == 0){
		return false;
	}

	if(this->isOpened()){
		return true;
	}
	
	return unzLocateFile(this->handle, this->path().c_str(), 0) == UNZ_OK;
}



std::vector<std::string> ZipFile::listDirContents(size_t maxEntries)const{
	if(!this->isDir()){
		throw utki::illegal_state("ZipFile::ListDirContents(): this is not a directory");
	}

	//if path refers to directory then there should be no files opened
	ASSERT(!this->isOpened())

	if(!this->handle){
		throw utki::illegal_state("ZipFile::ListDirContents(): zip file is not opened");
	}

	std::vector<std::string> files;

	//for every file, check if it is in the current directory
	{
		//move to first file
		int ret = unzGoToFirstFile(this->handle);

		if(ret != UNZ_OK){
			throw papki::exception("ZipFile::ListDirContents(): unzGoToFirstFile() failed.");
		}

		do{
			std::array<char, 255> fileNameBuf;

			if(unzGetCurrentFileInfo(
					this->handle,
					NULL,
					&*fileNameBuf.begin(),
					uLong(fileNameBuf.size()),
					NULL,
					0,
					NULL,
					0
				) != UNZ_OK)
			{
				throw papki::exception("ZipFile::ListDirContents(): unzGetCurrentFileInfo() failed.");
			}

			fileNameBuf[fileNameBuf.size() - 1] = 0;//null-terminate, just to be on a safe side

			std::string fn(&*fileNameBuf.begin());//filename

			if(fn.size() <= this->path().size()){
				continue;
			}

			//check if full file path starts with the this->Path() string
			if(fn.compare(0, this->path().size(), this->path()) != 0){
				continue;
			}

			ASSERT(fn.size() > this->path().size())
			std::string subfn(fn, this->path().size(), fn.size() - this->path().size());//subfilename

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
			throw papki::exception("ZipFile::ListDirContents(): unzGoToNextFile() failed.");
		}
	}

	std::vector<std::string> filesArray(files.size());
	for(unsigned i = 0; i < files.size(); ++i){
		filesArray[i] = files[i];
	}

	return filesArray;
}