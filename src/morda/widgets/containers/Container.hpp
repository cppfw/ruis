/* The MIT License:

Copyright (c) 2012-2015 Ivan Gagis <igagis@gmail.com>

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


namespace morda{



class Container : virtual public Widget{

private:
	T_ChildrenList children;
	
	//Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	typedef std::map<unsigned, std::pair<std::weak_ptr<Widget>, unsigned> > T_MouseCaptureMap;
	T_MouseCaptureMap mouseCaptureMap;
	
	//flag indicating that modifications to children list are blocked
	bool isBlocked = false;
	
protected:
	virtual std::unique_ptr<LayoutParams> CreateLayoutParams(const stob::Node* chain = nullptr)const{
		return Widget::LayoutParams::New(chain);
	}
	
	virtual LayoutParams& GetLayoutParams(Widget& w);
	
	const LayoutParams& GetLayoutParams(const Widget& w)const{
		return const_cast<std::remove_const<std::remove_pointer<decltype(this)>::type>::type*>(this)->GetLayoutParams(
				const_cast<std::remove_const<std::remove_reference<decltype(w)>::type>::type&>(w)
			);
	}
	
	template <class T> const T& GetLayoutParamsAs(const Widget& w)const{
		const auto lp = &this->GetLayoutParams(w);
		
		const auto p = dynamic_cast<const T*>(lp);
		if(!p){
			throw Exc("Container::GetLayoutParamsAs(): could not cast widget's layout parameters to a requested class");
		}
		return *p;
	}
	
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const;
	
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
	
	T_ChildrenList::iterator Add(const std::shared_ptr<Widget>& w, const Widget* insertBefore = nullptr);

	void Add(const stob::Node& chain);
	
	std::shared_ptr<Widget> Remove(Widget& w);
	
	void removeAll();
	
	std::shared_ptr<Widget> FindChildByName(const std::string& name)NOEXCEPT override;
	
	const T_ChildrenList& Children()const NOEXCEPT{
		return this->children;
	}
	
	virtual void OnChildrenListChanged(){}
};



}//~namespace
