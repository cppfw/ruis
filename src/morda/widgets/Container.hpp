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

#include "Widget.hpp"



namespace morda{



class Layout;



class Container : virtual public Widget{
private:
	ting::Ref<Widget> childrenHead, childrenTail;
	ting::Inited<unsigned, 0> numChildren;
	
	//Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	typedef std::map<unsigned, std::pair<ting::WeakRef<Widget>, unsigned> > T_MouseCaptureMap;
	T_MouseCaptureMap mouseCaptureMap;
	
protected:
	static const char* D_Layout()throw(){
		return "layout";
	}
	
	Container() : Widget(0){}
	
	Container(const stob::Node& description);

public:
	~Container()throw(){}
	
	static ting::Ref<Container> New(const stob::Node& description){
		return ting::Ref<Container>(new Container(description));
	}
	
	static ting::Ref<Container> New(){
		return ting::Ref<Container>(new Container());
	}
	
	void Render(const morda::Matr4f& matrix)const OVERRIDE;

	bool OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId) OVERRIDE;
public:
	
	bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId) OVERRIDE;
	
	void OnHoverChanged() OVERRIDE;
	
	void OnResize() OVERRIDE;

	void Add(const ting::Ref<Widget>& w);

	//return true if the widget was found in children and was removed
	void Remove(const ting::Ref<Widget>& w);
	
	ting::Ref<Widget> FindChildByName(const std::string& name)throw() OVERRIDE;
	
	unsigned NumChildren()const throw(){
		return this->numChildren;
	}
	
	const ting::Ref<Widget>& Children()throw(){
		return this->childrenHead;
	}
	
	const ting::Ref<const Widget>& Children()const throw(){
		return this->childrenHead;
	}
};



}//~namespace
