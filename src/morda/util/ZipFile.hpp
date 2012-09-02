/* The MIT License:

Copyright (c) 2009-2011 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

/**
 * @file Zip file reader
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <unzip/unzip.h>

#include <ting/debug.hpp>
#include <ting/fs/File.hpp>



class ZipFile : public ting::fs::File{
	std::string zipFilePath;

	unzFile zipFile; //ZIP file handle (unzFile is actually a pointer)
public:
	//TODO: should accept ting::File from where to read the zip file data
	ZipFile(const std::string& pathToZipFile = std::string(), const std::string& path = std::string()) :
			File(path),
			zipFilePath(pathToZipFile.c_str()),//copy
			zipFile(0)
	{}
	


	~ZipFile()throw(){
		this->CloseZipFile();
	}



	void OpenZipFile(){
		if(this->ZipFileIsOpened()){
			throw File::Exc("ZipFile::OpenZipFile(): zip file is already opened");
		}

		ASSERT(!this->IsOpened())

		if(this->zipFilePath.size() == 0){
			throw File::Exc("ZipFile::OpenZipFile(): zip file path is not set");
		}

		this->zipFile = unzOpen(this->zipFilePath.c_str());

		if(!this->zipFile){
			throw File::Exc("ZipFile::OpenZipFile(): opening zip file failed");
		}
	}



	void CloseZipFile()throw(){
		if(!this->ZipFileIsOpened()){
			return;
		}

		//make sure there is no file opened inside zip file
		this->Close();

		if(unzClose(this->zipFile) != UNZ_OK){
			ASSERT(false)
		}

		this->zipFile = 0;
	}



	inline bool ZipFileIsOpened()const throw(){
		return this->zipFile != 0;
	}



	inline void SetZipFilePath(const std::string &path){
		if(this->ZipFileIsOpened()){
			throw File::Exc("ZipFile::SetZipFilePath(): cannot set zip file path while the zip file is opened");
		}
		this->zipFilePath = std::string(path.c_str());//immediate copy (we do not want copy-on-write)
	}



	inline std::string GetZipFilePath()const{
		return this->zipFilePath;
	}



	//override
	virtual void Open(EMode mode){
		if(!this->ZipFileIsOpened()){
			throw File::Exc("ZipFile::Open(): zip file is not opened");
		}

		if(this->IsOpened()){
			throw File::Exc("file already opened");
		}

		if(mode != File::READ){
			throw File::Exc("illegal mode requested, only READ supported inside ZIP file");
		}

		if(unzLocateFile(this->zipFile, this->Path().c_str(), 0) != UNZ_OK){
			throw File::Exc("file not found");
		}

		{
			unz_file_info zipFileInfo;

			if(unzGetCurrentFileInfo(this->zipFile, &zipFileInfo, 0, 0, 0, 0, 0, 0) != UNZ_OK)
				throw File::Exc("failed obtaining file info");
		}

		if(unzOpenCurrentFile(this->zipFile) != UNZ_OK){
			throw File::Exc("file opening failed");
		}

		//set open mode
		this->ioMode = mode;

		this->isOpened = true;//set "opened" flag
	}



	//override
	virtual void Close()throw(){
		if(!this->IsOpened()){
			return;
		}

		ASSERT(this->ZipFileIsOpened())

		if(unzCloseCurrentFile(this->zipFile) == UNZ_CRCERROR){
			TRACE(<< "[WARNING] ZipFile::Close(): CRC is not good" << std::endl)
		}

		this->isOpened = false;
	}



	//override
	virtual size_t ReadInternal(ting::Buffer<ting::u8>& buf){
		int numBytesRead = unzReadCurrentFile(this->zipFile, buf.Begin(), buf.Size());
		if(numBytesRead < 0){
			throw File::Exc("ZipFile::Read(): file reading failed");
		}

		ASSERT(numBytesRead >= 0)
		return size_t(numBytesRead);
	}



	//override
	//returns number of bytes actually written
	virtual size_t WriteInternal(const ting::Buffer<ting::u8>& buf){
		if(this->ioMode != WRITE){
			throw File::Exc("file is opened, but not in WRITE mode");
		}

		ASSERT_INFO(false, "writing is not supported for zip files")
	}



	//override
	virtual bool Exists()const{
		if(this->Path().size() == 0){
			return false;
		}

		if(this->IsOpened()){
			return true;
		}

		//if it is a directory, check directory existance
		if(this->Path()[this->Path().size() - 1] == '/'){
			ASSERT(false)
		}else{
			return unzLocateFile(this->zipFile, this->Path().c_str(), 0) == UNZ_OK;
		}
	}



	//override
	virtual ting::Array<std::string> ListDirContents(size_t maxEntries = 0){
		if(!this->IsDir()){
			throw File::Exc("ZipFile::ListDirContents(): this is not a directory");
		}

		//if path refers to directory then there should be no files opened
		ASSERT(!this->IsOpened())

		if(!this->zipFile){
			throw File::Exc("ZipFile::ListDirContents(): zip file is not opened");
		}

		std::vector<std::string> files;

		//for every file, check if it is in the current directory
		{
			//move to first file
			int ret = unzGoToFirstFile(this->zipFile);

			if(ret != UNZ_OK){
				throw File::Exc("ZipFile::ListDirContents(): unzGoToFirstFile() failed.");
			}

			do{
				ting::StaticBuffer<char, 255> fileNameBuf;

				if(unzGetCurrentFileInfo(
						this->zipFile,
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
			}while((ret = unzGoToNextFile(this->zipFile)) == UNZ_OK);

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
};



