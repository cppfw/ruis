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
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>
#include <list>

#include "../../Exc.hpp"
#include "../Widget.hpp"

#include "../../util/LayoutParams.hpp"


namespace morda{



class Container : virtual public Widget{
	friend class Widget;
private:
	T_ChildrenList children;
	
	//Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	typedef std::map<unsigned, std::pair<std::weak_ptr<Widget>, unsigned> > T_MouseCaptureMap;
	T_MouseCaptureMap mouseCaptureMap;
	
	//flag indicating that modifications to children list are blocked
	bool isBlocked = false;
	
protected:
	virtual std::unique_ptr<LayoutParams> CreateLayoutParams(const stob::Node* chain = nullptr)const{
		throw Exc("Container::CreateLayoutParams(): simple Container cannot have layout params");
	}
	
	const LayoutParams& GetLayoutParams(Widget& w)const;
	
	template <class T> const T& GetLayoutParamsAs(Widget& w)const{
		auto p = dynamic_cast<const T*>(&this->GetLayoutParams(w));
		if(!p){
			throw Exc("Container::GetLayoutParamsAs(): could not cast widget's layout parameters to a requested class");
		}
		return *p;
	}
	
public:
	Container(const stob::Node* chain = nullptr);

public:
	~Container()NOEXCEPT{}
	
	void Render(const morda::Matr4r& matrix)const override;

	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) override;
public:
	
	bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) override;
	
	void OnHoverChanged(unsigned pointerID)override;
	
	void OnResize()override;

	void Add(const std::shared_ptr<Widget>& w);

	void Add(const stob::Node& chain);
	
	//return true if the widget was found in children and was removed
	void Remove(Widget& w);
	
	//TODO: RemoveAll()
	
	std::shared_ptr<Widget> FindChildByName(const std::string& name)NOEXCEPT override;
	
	const T_ChildrenList& Children()const NOEXCEPT{
		return this->children;
	}
	
	virtual void OnChildrenListChanged(){}
	
private:
	void MakeChildTopmost(Widget& w);
};



}//~namespace
