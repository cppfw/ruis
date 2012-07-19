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
#include <sstream>

#include <ting/Ref.hpp>
#include <ting/fs/File.hpp>

#include <stob/dom.hpp>


namespace morda{

//forward declarations
class ResMan;


//base class for all resources
class Resource : virtual public ting::RefCounted{
	friend class ResMan;

	struct StringComparator{
		bool operator()(const std::string& s1, const std::string& s2)const{
			return s1.compare(s2) < 0;
		}
	};
	typedef std::map<const std::string&, ting::WeakRef<Resource>, StringComparator> T_ResMap;
	typedef T_ResMap::iterator T_ResMapIter;

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
	T_ResMapIter it;

protected:
	//this can only be used as a base class
	inline Resource(){}
public:
	virtual ~Resource()throw();
};



//
//
//      Resource manager class
//
//
class ResMan : public ting::Singleton<ResMan>{
    friend class Resource;

	typedef Resource::T_ResMap T_ResMap;
	typedef T_ResMap::iterator T_ResMapIter;
	
	ting::Ref<Resource::ResMapRC> resMap;

	class ResPackEntry{
	public:
		ting::Ptr<ting::fs::File> fi;
		ting::Ptr<stob::Node> resScript;
	};

	typedef std::vector<ResPackEntry> T_ResPackList;
	typedef T_ResPackList::iterator T_ResPackIter;

	//list of mounted resource packs
	T_ResPackList resPacks;


	class FindInScriptRet{
	public:
		FindInScriptRet(ResPackEntry* resPack, const stob::Node* element) :
				rp(resPack),
				e(element)
		{}

		ResPackEntry* rp;
		const stob::Node* e;

		inline bool IsValid()const{
			return this->e && this->rp;
		}
	};

	FindInScriptRet FindResourceInScript(const std::string& resName);

	template <class T> ting::Ref<T> FindResourceInResMap(const std::string& resName);

	//Add resource to resources map
	void AddResource(ting::Ref<Resource> res, const std::string& resName);

	
public:
	inline ResMan() :
			resMap(Resource::ResMapRC::New())
	{}

	virtual ~ResMan(){}

	void MountResPack(ting::Ptr<ting::fs::File> fi);

	template <class T> ting::Ref<T> Load(const std::string& resName);
};



template <class T> ting::Ref<T> ResMan::FindResourceInResMap(const std::string& resName){
	T_ResMapIter i = this->resMap->rm.find(resName);
	if(i != this->resMap->rm.end()){
		ting::Ref<Resource> r((*i).second);
		ASSERT(r.DynamicCast<T>().IsValid())
		return r.StaticCast<T>();
	}
	return ting::Ref<T>();//no resource found with given name, return invalid reference
}



template <class T> ting::Ref<T> ResMan::Load(const std::string& resName){
//	TRACE(<< "ResMan::Load(): enter" << std::endl)
	if(ting::Ref<T> r = this->FindResourceInResMap<T>(resName)){
//		TRACE(<< "ResManHGE::Load(): resource found in map" << std::endl)
		return r;
	}

	TRACE(<< "ResMan::Load(): searching for resource in script..." << std::endl)
	ting::fs::File* fi DEBUG_CODE(= 0);
	stob::Node* el;
	{
		FindInScriptRet ret = this->FindResourceInScript(resName);
		if(!ret.IsValid()){
			TRACE(<< "ResMan::Load(): there is no resource with given name, throwing exception" << std::endl)
			std::stringstream ss;
			ss << "ResMan::Load(): there is no resource with given name: " << resName;
			throw ting::Exc(ss.str().c_str());
		}
		fi = ASS(ret.rp)->fi.operator->();
		el = ret.e;
	}
	ASSERT(fi)

	TRACE(<< "ResMan::Load(): resource found in script" << std::endl)

	ting::Ref<T> resource = T::Load(el, *fi);

	this->AddResource(resource, resName);

	TRACE(<< "ResMan::LoadTexture(): exit" << std::endl)
	return resource;
}



}//~namespace
