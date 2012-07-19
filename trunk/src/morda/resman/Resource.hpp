/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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

#include <map>

#include <ting/Ref.hpp>



namespace morda{

//forward declarations
class ResourceManager;


//base class for all resources
class Resource : virtual public ting::RefCounted{
	friend class ResourceManager;

	struct StringComparator{
		bool operator()(const std::string* s1, const std::string* s2)const{
			return s1->compare(*s2) < 0;
		}
	};
	typedef std::map<const std::string*, ting::WeakRef<Resource>, StringComparator> T_ResMap;

	//ResMapRC = Resource Map RefCounted
	class ResMapRC : public RefCounted{
	public:
		~ResMapRC()throw(){}
		
		T_ResMap rm;

		static inline ting::Ref<ResMapRC> New(){
			return ting::Ref<ResMapRC>(new ResMapRC());
		}
	};

	ting::WeakRef<ResMapRC> rm;
	T_ResMap::iterator it;

protected:
	//this can only be used as a base class
	inline Resource(){}
public:
	virtual ~Resource()throw();
};



}//~namespace
