/* The MIT License:

Copyright (c) 2009-2014 Ivan Gagis

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

#include <ting/debug.hpp>
#include <ting/fs/File.hpp>
#include <ting/util.hpp>
#include <ting/Ptr.hpp>
#include <ting/types.hpp>


namespace morda{


class ZipFile : public ting::fs::File{
	ting::Ptr<ting::fs::File> zipFile;
	
	ting::Inited<void*, 0> handle;
public:
	ZipFile(ting::Ptr<ting::fs::File> zipFile, const std::string& path = std::string());

	~ZipFile()throw();


	void OpenInternal(E_Mode mode) OVERRIDE;
	void CloseInternal() OVERRIDE;
	size_t ReadInternal(const ting::Buffer<ting::u8>& buf) OVERRIDE;
	bool Exists() const OVERRIDE;




	virtual ting::Array<std::string> ListDirContents(size_t maxEntries = 0) OVERRIDE{
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
};



}
