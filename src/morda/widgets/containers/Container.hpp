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
	virtual std::unique_ptr<LayoutParams> createLayoutParams(const stob::Node* chain = nullptr)const{
		return Widget::LayoutParams::New(chain);
	}
	
	virtual LayoutParams& getLayoutParams_internal(Widget& w);
	
	const LayoutParams& getLayoutParams_internal(const Widget& w)const{
		return const_cast<std::remove_const<std::remove_pointer<decltype(this)>::type>::type*>(this)->getLayoutParams_internal(
				const_cast<std::remove_const<std::remove_reference<decltype(w)>::type>::type&>(w)
			);
	}
	
	template <class T> const T& getLayoutParamsAs(const Widget& w)const{
		auto p = dynamic_cast<const T*>(&this->getLayoutParams_internal(w));
		if(!p){
			w.layoutParams.reset();
			p = dynamic_cast<const T*>(&this->getLayoutParams_internal(w));
		}
		
		ASSERT(p)
		return *p;
	}
	
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const;
	
public:
	LayoutParams& getLayoutParams(Widget& w){
		if(w.parent() != this){
			throw Exc("Container::getLayoutParams(): the widget is not added to this container");
		}		
		this->SetRelayoutNeeded();
		return this->getLayoutParams_internal(w);
	}
	
	
public:
	Container(const stob::Node* chain = nullptr);

public:
	~Container()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;

	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) override;
public:
	
	bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) override;
	
	void OnHoverChanged(unsigned pointerID)override;
	
	void layOut()override;
	
	T_ChildrenList::iterator Add(const std::shared_ptr<Widget>& w, T_ChildrenList::const_iterator insertBefore);
	
	T_ChildrenList::iterator Add(const std::shared_ptr<Widget>& w, const Widget* insertBefore = nullptr);

	void Add(const stob::Node& chain);
	
	std::shared_ptr<Widget> Remove(T_ChildrenList::const_iterator iter);
	
	std::shared_ptr<Widget> Remove(Widget& w);
	
	void removeAll();
	
	std::shared_ptr<Widget> FindChildByName(const std::string& name)noexcept override;
	
	const T_ChildrenList& Children()const noexcept{
		return this->children;
	}
	
	/**
	 * @brief Called when children list changes.
	 * Default implementation requests re-layout.
     */
	virtual void onChildrenListChanged(){
		this->SetRelayoutNeeded();
	}
};




template <class T> T* Widget::findParent(const char* name){
	if(!this->parent()){
		return nullptr;
	}

	auto p = dynamic_cast<T*>(this->parent());
	if(p && (!name || p->name() == name)){
		return p;
	}

	return this->parent()->findParent<T>();
}



}//~namespace
