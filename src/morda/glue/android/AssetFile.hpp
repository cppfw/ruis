/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

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

// Home page: http://morda.googlecode.com

/**
 * @file Android assets File implementation
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#ifndef __ANDROID__
#	error "Compiling in non-Android environment"
#endif

#include <android/asset_manager.h>

#include <ting/debug.hpp>
#include <ting/Ptr.hpp>
#include <ting/fs/File.hpp>

#include "../../App.hpp"



namespace morda{



/**
 * @brief Android assets implementation of File interface.
 * Implementation of a ting::fs::File interface for Android assets file system.
 */
class AssetFile : public ting::fs::File{
	AAssetManager* manager;
	
	ting::Inited<AAsset*, 0> handle;
	
	AssetFile(AAssetManager* manager, const std::string& pathName = std::string()) :
			manager(manager),
			File(pathName)
	{
		ASSERT(this->manager)
	}
	
	static inline ting::Ptr<AssetFile> New(AAssetManager* manager, const std::string& pathName = std::string()){
		return ting::Ptr<AssetFile>(new AssetFile(manager, pathName));
	}
	
	friend ting::Ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path = std::string())const;
	
public:
	/**
	 * @brief Destructor.
	 * This destructor calls the Close() method.
	 */
	~AssetFile()throw(){
		this->Close();
	}


	//override
	virtual void OpenInternal(E_Mode mode);



	//override
	virtual void CloseInternal()throw();



	//override
	virtual size_t ReadInternal(const ting::Buffer<ting::u8>& buf);



	//override
	virtual size_t WriteInternal(const ting::Buffer<const ting::u8>& buf);



	//override
	virtual void SeekForwardInternal(size_t numBytesToSeek);
	
	
	
	//override
	virtual void SeekBackwardInternal(size_t numBytesToSeek);

	
	
	/**
	 * @brief Seek forward or backward.
     * @param numBytesToSeek - number of bytes to seek.
     * @param seekForward - if true, then the seeking will be done forward.
	 *                      If false, then the seeking will be done backward.
     */
	void Seek(size_t numBytesToSeek, bool seekForward);
	
	
	//override
	virtual void RewindInternal();
	


	//override
	virtual bool Exists()const;



public:

	//override
	virtual ting::Array<std::string> ListDirContents(size_t maxEntries = 0);
};



}//~namespace
