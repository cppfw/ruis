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

#include <string>

#include <ting/types.hpp>
#include <ting/Ref.hpp>

#include "util/Matrix4.hpp"
#include "util/Vector2.hpp"
#include "util/Rectangle2.hpp"

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public ting::RefCounted{
	friend class Container;
	friend class App;
	
	ting::Ref<Widget> next, prev;
	
private:
	ting::WeakRef<Container> parent;
	
	ting::Inited<bool, false> isHovered;

	void Unhover();

	ting::Inited<bool, false> isHidden;

	ting::Inited<bool, false> isDisabled;

	morda::Rect2f rect;
	
	std::string name;
	
	//Apply description from STOB
	void ApplyDescription(const stob::Node& description);
	
	ting::Inited<bool, true> relayoutNeeded;
	
public:
	ting::Ptr<stob::Node> prop;
	
	const std::string& Name()const throw(){
		return this->name;
	}
	
	inline const ting::WeakRef<Container>& Parent()throw(){
		return this->parent;
	}
	
	inline ting::WeakRef<const Container> Parent()const throw(){
		return this->parent;
	}
	
	inline const ting::Ref<Widget>& Next()throw(){
		return this->next;
	}
	
	inline const ting::Ref<const Widget>& Next()const throw(){
		return this->next;
	}
	
	inline const ting::Ref<Widget>& Prev()throw(){
		return this->prev;
	}
	
	inline const ting::Ref<const Widget>& Prev()const throw(){
		return this->prev;
	}
	
	//NOTE: if only parent holds Ref then object may be deleted
	void RemoveFromParent();
	
	inline bool IsHovered()const throw(){
		return this->isHovered;
	}

	inline const morda::Rect2f& Rect()const throw(){
		return this->rect;
	}

	inline void SetPos(const morda::Vec2f& newPos)throw(){
		this->rect.p = newPos;
	}
	
	inline void Move(const morda::Vec2f& delta)throw(){
		this->rect.p += delta;
	}

	inline void Resize(const morda::Vec2f& newDims){
		this->rect.d = newDims;
		this->OnResize();//call virtual method
		this->relayoutNeeded = false;
	}

protected:
	inline Widget(){}
	
	inline Widget(const stob::Node& description){
		this->ApplyDescription(description);
	}
	
public:
	static ting::Ref<Widget> New(const stob::Node& description){
		return ting::Ref<Widget>(new Widget(description));
	}
	
	static ting::Ref<Widget> New(){
		return ting::Ref<Widget>(new Widget());
	}

	virtual ~Widget()throw(){}

	virtual void Render(const morda::Matr4f& matrix)const;

	enum EMouseButton{
		UNKNOWN,
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN
	};

	//return true to consume event
	virtual bool OnMouseButtonDown(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
		return false;
	}
	
	//return true to consume event
	virtual bool OnMouseButtonUp(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
		return false;
	}

	//return true to consume event
	virtual bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
		return false;
	}

	virtual void OnMouseOut(){
//		TRACE(<< "Widget::OnMouseOut(): invoked" << std::endl)
	}

	virtual void OnMouseIn(){
//		TRACE(<< "Widget::OnMouseIn(): invoked" << std::endl)
	}

	virtual void OnResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
	}
	
	virtual morda::Vec2f ComputeMinimalDimensions()const throw(){
		return this->Rect().d;
	}

	void RelayoutNeeded()throw();
	
	inline void Hide(){
		this->SetHidden(true);
	}

	inline void Show(){
		this->SetHidden(false);
	}

	inline void SetHidden(bool hidden){
		this->isHidden = hidden;
		if(this->isHidden && this->isHovered){
			this->isHovered = false;
			this->OnMouseOut();
		}
	}
	
	inline bool IsHidden()const throw(){
		return this->isHidden;
	}

	inline void Enable()throw(){
		this->SetDisabled(false);
	}

	inline void Disable()throw(){
		this->SetDisabled(true);
	}

	inline void SetDisabled(bool disabled)throw(){
		this->isDisabled = disabled;
	}
	
	inline bool IsDisabled()const throw(){
		return this->isDisabled;
	}

protected:
};



}//~namespace
