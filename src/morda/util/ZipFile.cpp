#include "ZipFile.hpp"

#include "unzip/unzip.h"

#include <sstream>


using namespace morda;



ZipFile::ZipFile(ting::Ptr<ting::fs::File> zipFile, const std::string& path) :
		ting::fs::File(path),
		zipFile(zipFile)
{
	this->handle = unzOpen(this->zipFile->Path().c_str());

	if(!this->handle){
		throw File::Exc("ZipFile::OpenZipFile(): opening zip file failed");
	}
}



ZipFile::~ZipFile()throw(){
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

void ZipFile::CloseInternal()throw(){
	if(unzCloseCurrentFile(this->handle) == UNZ_CRCERROR){
		TRACE(<< "[WARNING] ZipFile::Close(): CRC is not good" << std::endl)
		ASSERT(false)
	}
}

size_t ZipFile::ReadInternal(const ting::Buffer<ting::u8>& buf) {
	int numBytesRead = unzReadCurrentFile(this->handle, buf.Begin(), buf.Size());
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



ting::Array<std::string> ZipFile::ListDirContents(size_t maxEntries){
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
			ting::StaticBuffer<char, 255> fileNameBuf;

			if(unzGetCurrentFileInfo(
					this->handle,
					NULL,
					fileNameBuf.Begin(),
					fileNameBuf.Size(),
					NULL,
					0,
					NULL,
					0
				) != UNZ_OK)
			{
				throw File::Exc("ZipFile::ListDirContents(): unzGetCurrentFileInfo() failed.");
			}

			fileNameBuf[fileNameBuf.Size() - 1] = 0;//null-terminate, just to be on a safe side

			std::string fn(fileNameBuf.Begin());//filename

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

	ting::Array<std::string> filesArray(files.size());
	for(unsigned i = 0; i < files.size(); ++i){
		filesArray[i] = files[i];
	}

	return filesArray;
}