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

	~ZipFile()noexcept;


	void OpenInternal(E_Mode mode) override;
	void CloseInternal()noexcept override;
	size_t ReadInternal(const ting::Buffer<std::uint8_t>& buf) override;
	bool Exists() const override;
	ting::Array<std::string> ListDirContents(size_t maxEntries = 0) override;
	
	static ting::Ptr<ZipFile> New(ting::Ptr<ting::fs::File> zipFile, const std::string& path = std::string()){
		return ting::Ptr<ZipFile>(new ZipFile(zipFile, path));
	}
};



}
