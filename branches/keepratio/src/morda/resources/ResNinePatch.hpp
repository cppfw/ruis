/* The MIT License:

Copyright (c) 2014 Ivan Gagis <igagis@gmail.com>

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

#include "../ResourceManager.hpp"
#include "ResImage.hpp"

namespace morda{

class ResNinePatch : public Resource{
	friend class ResourceManager;
public:
	const std::shared_ptr<ResImage> lt, t, rt, l, m, r, lb, b, rb;
	
	ResNinePatch(const ResNinePatch&) = delete;
	ResNinePatch& operator=(const ResNinePatch&) = delete;
	
	ResNinePatch(
			const std::shared_ptr<ResImage>& lt,
			const std::shared_ptr<ResImage>& t,
			const std::shared_ptr<ResImage>& rt,
			const std::shared_ptr<ResImage>& l,
			const std::shared_ptr<ResImage>& m,
			const std::shared_ptr<ResImage>& r,
			const std::shared_ptr<ResImage>& lb,
			const std::shared_ptr<ResImage>& b,
			const std::shared_ptr<ResImage>& rb
		) :
			lt(lt),
			t(t),
			rt(rt),
			l(l),
			m(m),
			r(r),
			lb(lb),
			b(b),
			rb(rb)
	{}
private:
	static std::shared_ptr<ResNinePatch> Load(const stob::Node& chain, const ting::fs::File& fi);
};

}
