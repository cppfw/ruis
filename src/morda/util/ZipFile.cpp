#include "ZipFile.hpp"

#include <unzip/unzip.h>



using namespace morda;



ZipFile::ZipFile(ting::Ptr<ting::fs::File> zipFile, const std::string& path) :
		ting::fs::File(path),
		zipFile(zipFile)
{
	this->handle = unzOpen(this->zipFilePath.c_str());

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
		throw File::Exc("file not found");
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

void ZipFile::CloseInternal() {
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


