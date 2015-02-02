/* The MIT License:

Copyright (c) 2015 Ivan Gagis <igagis@gmail.com>

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
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "LinearContainer.hpp"

namespace morda{

class TableRow : public HorizontalContainer{
public:
	TableRow(const stob::Node* chain = nullptr);
	
	TableRow(const TableRow&) = delete;
	TableRow& operator=(const TableRow&) = delete;
	
	class LayoutParams : public HorizontalContainer::LayoutParams{
		HorizontalContainer::LayoutParams modifiedParams;
	protected:
		LayoutParams(const stob::Node* chain);
	public:
		real weight;
		
		static std::unique_ptr<LayoutParams> New(const stob::Node* chain = nullptr){
			return std::unique_ptr<LayoutParams>(new LayoutParams(chain));
		}
	};
	
private:
	std::unique_ptr<morda::LayoutParams> CreateLayoutParams(const stob::Node* chain)const override{
		return LayoutParams::New(chain);
	}
};

}
