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

#include <tride/Vector3.hpp>
#include <tride/Rectangle2.hpp>

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public ting::RefCounted{
	friend class morda::Container;
	
private:
	ting::WeakRef<Container> parent;
	
	ting::Inited<bool, false> isHovered;

	void Unhover();

	ting::Inited<bool, false> isHidden;

	ting::Inited<bool, false> isDisabled;

	tride::Rect2f rect;
	
	std::string name;
public:

	ting::Ptr<stob::Node> properties;
	
	const std::string& Name()const throw(){
		return this->name;
	}
	
	inline const ting::WeakRef<Container>& Parent()throw(){
		return this->parent;
	}
	
	inline ting::WeakRef<const Container> Parent()const throw(){
		return this->parent;
	}
	
	void RemoveFromParent();
	
	inline const stob::Node* Properties()const throw(){
		return this->properties.operator->();
	}
	
	inline bool IsHovered()const throw(){
		return this->isHovered;
	}

	inline const tride::Rect2f& Rect()const throw(){
		return this->rect;
	}

	inline void SetPos(const tride::Vec2f& newPos)throw(){
		this->rect.p = newPos;
	}
	
	inline void Move(const tride::Vec2f& delta)throw(){
		this->rect.p += delta;
	}

	inline void Resize(const tride::Vec2f& newDims){
		this->rect.d = newDims;
		this->OnResize();//call virtual method
	}

protected:
	inline Widget(ting::Ptr<stob::Node> properties) :
			properties(properties)
	{
		this->ApplyProperties(this->properties.operator->());
	}
	
	inline Widget(const stob::Node* properties = 0){
		this->ApplyProperties(properties);
	}
	
private:
	void ApplyProperties(const stob::Node* properties);

public:
	static ting::Ref<Widget> New(ting::Ptr<stob::Node> properties){
		return ting::Ref<Widget>(new Widget(properties));
	}
	
	static ting::Ref<Widget> New(const stob::Node* properties = 0){
		return ting::Ref<Widget>(new Widget(properties));
	}

	virtual ~Widget()throw(){}

	virtual void Render(const tride::Matr4f& matrix)const{}

	enum EMouseButton{
		UNKNOWN,
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN
	};

	//return true to consume event
	virtual bool OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
		return false;
	}
	
	//return true to consume event
	virtual bool OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
		return false;
	}

	//return true to consume event
	virtual bool OnMouseMove(const tride::Vec2f& pos, unsigned pointerId){
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
