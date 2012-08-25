/* The MIT License:

Copyright (c) 2011 Ivan Gagis

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

// Homepage: http://ting.googlecode.com



#ifdef WIN32
#include <windows.h>

#elif defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#include <cerrno>

#endif

#include <vector>
#include <cstdlib>

#include "AssetFile.hpp"



using namespace ting_android;



//override
void AssetFile::Open(E_Mode mode){
	if(this->IsOpened()){
		throw File::Exc("file already opened");
	}

	if(this->IsDir()){
		throw File::Exc("path refers to a directory, directories can't be opened");
	}
	

	switch(mode){
		case File::WRITE:
		case File::CREATE:
			throw File::Exc("WRITE and CREATE open modes are not supported by Android assets");
			break;
		case File::READ:
			break;
		default:
			throw File::Exc("unknown mode");
			break;
	}
	this->handle = AAssetManager_open(this->manager, this->Path().c_str(), AASSET_MODE_UNKNOWN); //don't know what this MODE mean at all
	if(!this->handle){
		throw File::Exc("AAssetManager_open() failed");
	}

	this->ioMode = READ;
	
	this->isOpened = true;//set "opened" flag
}



//override
void AssetFile::Close()throw(){
	if(!this->IsOpened())
		return;

	ASSERT(this->handle)

	AAsset_close(this->handle);
	this->handle = 0;

	this->isOpened = false;
}



//override
size_t AssetFile::ReadInternal(ting::Buffer<ting::u8>& buf){
	ASSERT(this->handle)
	int numBytesRead = AAsset_read(this->handle, buf.Begin(), buf.Size());
	if(numBytesRead < 0){//something happened
		throw File::Exc("AAsset_read() error");
	}
	ASSERT(numBytesRead >= 0)
	return size_t(numBytesRead);
}



//override
size_t AssetFile::WriteInternal(const ting::Buffer<ting::u8>& buf){
	ASSERT(this->handle)
	throw File::Exc("Write() is not supported by Android assets");
	return 0;
}



void AssetFile::Seek(size_t numBytesToSeek, bool seekForward){
	if(!this->IsOpened()){
		throw File::Exc("file is not opened, cannot seek");
	}

	ASSERT(this->handle)
	
	//NOTE: AAsset_seek() accepts 'off_t' as offset argument which is signed and can be
	//      less than size_t value passed as argument to this function.
	//      Therefore, do several seek operations with smaller offset if necessary.
	
	typedef off_t T_FSeekOffset;
	const size_t DMax = ((size_t(T_FSeekOffset(-1))) >> 1);
	ASSERT((size_t(1) << ((sizeof(T_FSeekOffset) * 8) - 1)) - 1 == DMax)
	STATIC_ASSERT(size_t(-(-T_FSeekOffset(DMax))) == DMax)
	
	for(size_t numBytesLeft = numBytesToSeek; numBytesLeft != 0;){
		ASSERT(numBytesLeft <= numBytesToSeek)
		
		T_FSeekOffset offset;
		if(numBytesLeft > DMax){
			offset = T_FSeekOffset(DMax);
		}else{
			offset = T_FSeekOffset(numBytesLeft);
		}
		
		ASSERT(offset > 0)
		
		if(AAsset_seek(this->handle, seekForward ? offset : (-offset), SEEK_CUR) < 0){
			throw File::Exc("AAsset_seek() failed");
		}
		
		ASSERT(size_t(offset) < size_t(-1))
		ASSERT(numBytesLeft >= size_t(offset))
		
		numBytesLeft -= size_t(offset);
	}
}



//override
void AssetFile::SeekForward(size_t numBytesToSeek){
	this->Seek(numBytesToSeek, true);
}



//override
void AssetFile::SeekBackward(size_t numBytesToSeek){
	this->Seek(numBytesToSeek, false);
}



//override
void AssetFile::Rewind(){
	if(!this->IsOpened()){
		throw File::Exc("file is not opened, cannot rewind");
	}

	ASSERT(this->handle)
	if(AAsset_seek(this->handle, 0, SEEK_SET) < 0){
		throw File::Exc("AAsset_seek() failed");
	}
}



//override
bool AssetFile::Exists()const{
	if(this->IsOpened()){ //file is opened => it exists
		return true;
	}
	
	if(this->Path().size() == 0){
		return false;
	}

	if(this->IsDir()){
		//try opening the directory to check its existence
		AAssetDir* pdir = AAssetManager_openDir(this->manager, this->Path().c_str());

		if(!pdir){
			return false;
		}else{
			AAssetDir_close(pdir);
			return true;
		}
	}else{
		return this->File::Exists();
	}
}



//override
ting::Array<std::string> AssetFile::ListDirContents(size_t maxEntries){
	if(!this->IsDir()){
		throw File::Exc("AndroidAssetFile::ListDirContents(): this is not a directory");
	}

	std::vector<std::string> files;

	{
		AAssetDir* pdir = AAssetManager_openDir(this->manager, this->Path().c_str());

		if(!pdir){
			throw File::Exc("AndroidAssetFile::ListDirContents(): AAssetManager_openDir() failure");
		}

		//create DirentCloser to automatically call closedir on exit from the function in case of exceptions etc...
		struct DirCloser{
			AAssetDir *pdir;

			DirCloser(AAssetDir *pDirToClose) :
					pdir(pDirToClose)
			{}

			~DirCloser(){
				AAssetDir_close(this->pdir);
			}
		} dirCloser(pdir);

		while(const char *entry = AAssetDir_getNextFileName(pdir)){
			std::string s(entry);
			if(s == "." || s == "..")
				continue;//do not add ./ and ../ directories, we are not interested in them

			//NOTE: looks like android does not report directories contained in the inspected directory.

			files.push_back(s);
			
			if(files.size() == maxEntries){
				break;
			}
		}//~while()
	}

	
	ting::Array<std::string> filesArray(files.size());
	
	for(size_t i = 0; i < files.size(); ++i){
		filesArray[i] = files[i];
	}

	return filesArray;
}//~ListDirContents()
