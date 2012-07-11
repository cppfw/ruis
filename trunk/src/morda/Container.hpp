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

#include <vector>

#include <ting/types.hpp>
#include <ting/Ref.hpp>
#include <ting/Signal.hpp>

#include <tride/Vector3.hpp>



class Widget : virtual public ting::RefCounted{
	friend class RootWidgetWrapper;

public:
	typedef std::vector<ting::Ref<Widget> > T_ChildList;
	typedef T_ChildList::iterator T_ChildIter;
	typedef T_ChildList::const_iterator T_ChildConstIter;

private:
	T_ChildList children;

	ting::WeakRef<Widget> parent;

	ting::Ref<Widget> modal;
	bool drawWithModal;

	ting::Inited<bool, false> isHovered;

	//return true if was hovered and was unhovered
	void Unhover();

	ting::Inited<bool, false> isHidden;

	ting::Inited<bool, false> isDisabled;

	tride::Vec2f p;
	tride::Vec2f d;
public:

	enum EPriority{
		ALWAYS_ON_BOTTOM,
		ALWAYS_IN_BETWEEN,
		ALWAYS_ON_TOP
	};
private:
	ting::Inited<EPriority, ALWAYS_IN_BETWEEN> priority;
public:
	void SetPriority(EPriority priority);

	inline EPriority Priority()const{
		return this->priority;
	}


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

	void ShowModal(ting::Ref<Widget> w, bool drawWidgetsBelowModal = true);

	void RemoveModal();

protected:
	Widget();

public:
	static ting::Ref<Widget> New(){
		return ting::Ref<Widget>(new Widget());
	}

	virtual ~Widget()throw(){}

	inline ting::WeakRef<Widget>& Parent(){
		return this->parent;
	}

	inline const ting::WeakRef<Widget>& Parent()const{
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

	void Add(ting::Ref<Widget> w);

	//return true if the widget was found in children and was removed
	bool Remove(ting::Ref<Widget> w);

	void RemoveFromParent();

	inline const T_ChildList Children()const{
		return this->children;
	}

	inline T_ChildList Children(){
		return this->children;
	}

	inline void Hide(){
		this->SetHidden(true);
	}

	inline void Show(){
		this->SetHidden(false);
	}



	inline void Enable(){
		this->SetDisabled(false);
	}

	inline void Disable(){
		this->SetDisabled(true);
	}

	inline void SetDisabled(bool isDisabled){
		this->isDisabled = isDisabled;
	}

	void RenderWithChildren(const tride::Matr4f& matrix)const;

protected:
	bool OnMouseClickInternal(const tride::Vec2f& pos, EMouseButton button, bool isDown);

	bool OnMouseMoveInternal(const tride::Vec2f& oldPos, const tride::Vec2f& newPos, const tride::Vec2f& dpos);

	/**
	 * @brief Check if point is in widget's rectangle.
	 * @param point - point to check, in widget parent's coordinates.
	 * @return true if point lies inside widgets rectangle.
	 * @return false otherwise.
	 */
	bool IsInWidgetRect(const tride::Vec2f& point){
		return
				this->p.x <= point.x &&
				point.x <= (this->p.x + this->d.x) &&
				this->p.y <= point.y &&
				point.y <= (this->p.y + this->d.y)
			;
	}
};



class RootWidgetWrapper{
	ting::Ref<Widget> w;

public:

	inline void SetWidget(ting::Ref<Widget> widget){
		this->w = widget;
	}


	inline ting::Ref<Widget> GetWidget(){
		return this->w;
	}


	inline void Render(const tride::Matr4f& matrix)const{
		if(this->w.IsNotValid()){
			return;
		}

//		TRACE(<< "RootWidgetWrapper::Render(): calling Render() on widget, matrix = " << matrix << std::endl)

		ting::Ref<Widget> widget(this->w);//make copy reference because this->w may be reset during the call
		ASS(widget)->RenderWithChildren(matrix);
	}

	inline void OnMouseClick(const tride::Vec2f& pos, Widget::EMouseButton button, bool isDown){
		if(this->w.IsNotValid())
			return;

		ting::Ref<Widget> widget(w);//make copy reference because this->w may be reset during the call
		ASS(widget)->OnMouseClickInternal(pos, button, isDown);
//		TRACE(<< "RootWidgetWrapper::OnMouseClick(): mouse click handled" << std::endl)
	}

	inline void OnMouseMove(const tride::Vec2f& oldPos, const tride::Vec2f& newPos, const tride::Vec2f& dpos){
//		TRACE(<< "RootWidgetWrapper::OnMouseMove(): dpos = " << dpos << std::endl)

		if(this->w.IsNotValid())
			return;

		ting::Ref<Widget> widget(w);//make copy reference because this->w may be reset during the call
		ASS(widget)->OnMouseMoveInternal(oldPos, newPos, dpos);
	}
};



class Button : public Widget{
protected:
	ting::Inited<bool, false> isPressed;

private:
	//override
	bool OnMouseClick(const tride::Vec2f& pos, Widget::EMouseButton button, bool isDown);

	//override
	void OnMouseOut();

	//override
	void Render(const tride::Matr4f& matrix)const;

protected:
	Button(){}
public:
	~Button()throw(){}

	ting::Signal0 clicked;

	static ting::Ref<Button> New(){
		return ting::Ref<Button>(new Button());
	}
};



class ToggleButton : public Widget{
protected:
	ting::Inited<bool, false> isPressed;

private:
	//override
	bool OnMouseClick(const tride::Vec2f& pos, Widget::EMouseButton button, bool isDown);

	//override
	void Render(const tride::Matr4f& matrix)const;

protected:
	ToggleButton(){}
public:
	
	~ToggleButton()throw(){}

	void SetPressed(bool isPressed);

	ting::Signal1<bool> clicked;

	static ting::Ref<ToggleButton> New(){
		return ting::Ref<ToggleButton>(new ToggleButton());
	}
};
