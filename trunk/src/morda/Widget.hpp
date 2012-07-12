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

#include <ting/types.hpp>
#include <ting/Ref.hpp>

#include <tride/Vector3.hpp>



namespace morda{



class Widget : virtual public ting::RefCounted{
private:
	ting::WeakRef<Widget> parent;//Container?

	ting::Inited<bool, false> isHovered;

	//return true if was hovered and was unhovered
	void Unhover();

	ting::Inited<bool, false> isHidden;

	ting::Inited<bool, false> isDisabled;

	tride::Vec2f p;
	tride::Vec2f d;
public:

	inline bool IsHovered()const{
		return this->isHovered;
	}

	inline const tride::Vec2f& Pos()const{
		return this->p;
	}

	inline const tride::Vec2f TopRightPos()const{
		return this->Pos() + this->Dim();
	}

	inline const tride::Vec2f& Dim()const{
		return this->d;
	}

	void Move(const tride::Vec2f& newPos);

	void Resize(const tride::Vec2f& newDims);

protected:
	inline Widget(){}

public:
	static ting::Ref<Widget> New(){
		return ting::Ref<Widget>(new Widget());
	}

	virtual ~Widget()throw(){}

	inline ting::WeakRef<Widget>& Parent(){
		return this->parent;
	}

	inline const ting::WeakRef<const Widget> Parent()const{
		return this->parent;
	}

	virtual void Render(const tride::Matr4f& matrix)const{}

	enum EMouseButton{
		LEFT = 1, //1b
		RIGHT = 2, //10b
		MIDDLE = 4 //100b
	};

	virtual bool OnMouseClick(const tride::Vec2f& pos, EMouseButton button, bool isDown){
		return false;
	}

	virtual bool OnMouseMove(const tride::Vec2f& oldPos, const tride::Vec2f& newPos, const tride::Vec2f& dpos){
		return false;
	}

	virtual void OnMouseOut(){}

	virtual void OnMouseIn(){}

	virtual void OnResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
	}

	void RemoveFromParent();

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

	inline void Enable(){
		this->SetDisabled(false);
	}

	inline void Disable(){
		this->SetDisabled(true);
	}

	inline void SetDisabled(bool disabled){
		this->isDisabled = disabled;
	}

protected:
	/**
	 * @brief Check if point is in widget's rectangle.
	 * @param point - point to check, in widget parent's coordinates.
	 * @return true if point lies inside widgets rectangle.
	 * @return false otherwise.
	 */
	bool IsInWidgetRect(const tride::Vec2f& point){
		return
				this->p.x <= point.x &&
				point.x < (this->p.x + this->d.x) &&
				this->p.y <= point.y &&
				point.y < (this->p.y + this->d.y)
			;
	}
};



}//~namespace
