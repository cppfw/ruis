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

#include <string>

#include <ting/types.hpp>
#include <ting/Shared.hpp>

#include "../util/keycodes.hpp"

#include "../util/Matrix4.hpp"
#include "../util/Vector2.hpp"
#include "../util/Rectangle2.hpp"

#include "../config.hpp"

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public ting::Shared{
	friend class Container;
	friend class App;
	
public:
	typedef std::list<std::shared_ptr<Widget>> T_ChildrenList;
	
private:
	Container* parent = nullptr;
	T_ChildrenList::iterator parentIter;
	
	ting::Inited<bool, false> isHovered;

	ting::Inited<bool, false> isHidden;

	ting::Inited<bool, false> isDisabled;

	morda::Rect2f rect;
	
	//minimal dimensions needed to show widget's contents normally
	morda::Vec2f minDim;
	ting::Inited<bool, true> minDimNeedsRecomputing;
	
	//clip widgets contents by widget's border if set to true
	ting::Inited<bool, false> clip;
public:
	bool GetClip()const noexcept{
		return this->clip;
	}
	
	void SetClip(bool clip)noexcept{
		this->clip = clip;
	}
	
private:
	//logical ID of the widget
	std::string name;
	
	bool relayoutNeeded = true;
	
	//properties from STOB description
	std::unique_ptr<stob::Node> prop;
public:
	const stob::Node* Prop()const noexcept{
		return this->prop.operator->();
	}
	
	const stob::Node* GetPropertyNode(const char* propName)const noexcept{
		if(!this->Prop()){
			return nullptr;
		}
		return this->Prop()->Child(propName).node();
	}
	
	std::unique_ptr<stob::Node> ExtractProp()noexcept{
		return std::move(this->prop);
	}
	
	void SetProp(std::unique_ptr<stob::Node> prop)noexcept{
		this->prop = std::move(prop);
	}
	
	bool NeedsRelayout()const noexcept{
		return this->relayoutNeeded;
	}
	
	const std::string& Name()const noexcept{
		return this->name;
	}
	
	const Container* Parent()const noexcept{
		return this->parent;
	}
	
	Container* Parent()noexcept{
		return this->parent;
	}
	
	//NOTE: if only parent holds Ref then object may be deleted
	void RemoveFromParent();
	
	bool IsHovered()const noexcept{
		return this->isHovered;
	}
	
private:
	void SetHovered(bool isHovered){
		if(this->isHovered == isHovered){
			return;
		}
		
		this->isHovered = isHovered;
		this->OnHoverChanged();
	}
	
public:

	const morda::Rect2f& Rect()const noexcept{
		return this->rect;
	}
	
	void MoveTo(const morda::Vec2f& newPos)noexcept{
		this->rect.p = newPos;
	}
	
	void MoveBy(const morda::Vec2f& delta)noexcept{
		this->rect.p += delta;
	}

	void Resize(const morda::Vec2f& newDims){
		if(this->rect.d == newDims){
			return;
		}
		
		this->rect.d = newDims;
		this->OnResize();//call virtual method
		this->relayoutNeeded = false;
	}

	virtual std::shared_ptr<Widget> FindChildByName(const std::string& name)noexcept{
		return nullptr;
	}
	
public:
	//NOTE: dummy int argument to avoid implicit call of default constructor in derived classes, to avoid silent bugs.
	Widget(int dummy) :
			rect(0)
	{}
	
	Widget(const stob::Node& description);
	
public:

	virtual ~Widget()noexcept{}

	virtual void Render(const morda::Matr4f& matrix)const{};
	
private:
	void RenderInternal(const morda::Matr4f& matrix)const;
	
private:
	void OnKeyInternal(bool isDown, key::Key keyCode);
	
private:
	ting::Inited<bool, false> isFocused;
public:
	
	//return true to consume
	virtual bool OnKey(bool isDown, morda::key::Key keyCode){
		return false;
	}
	
	void Focus()noexcept;
	
	void Unfocus()noexcept;
	
	bool IsFocused()const noexcept{
		return this->isFocused;
	}
	
	//TODO: add on focus changed
	
	enum EMouseButton{
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN,
		
		ENUM_SIZE
	};

	std::function<bool (Widget& widget, bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId)> onMouseButton;
	
	//return true to consume event
	virtual bool OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
		if(this->onMouseButton){
			return this->onMouseButton(*this, isDown, pos, button, pointerId);
		}
		return false;
	}

	std::function<bool (Widget& widget, const morda::Vec2f& pos, unsigned pointerId)> onMouseMove;
	
	//return true to consume event
	virtual bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
		if(this->onMouseMove){
			return this->onMouseMove(*this, pos, pointerId);
		}
		return false;
	}

	virtual void OnHoverChanged(){
//		TRACE(<< "Widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	virtual void OnResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
	}
	
	const morda::Vec2f& GetMinDim()const{
		if(this->minDimNeedsRecomputing){
			const_cast<Widget*>(this)->minDim = this->ComputeMinDim();
			this->minDimNeedsRecomputing = false;
		}
		return this->minDim;
	}

	virtual morda::Vec2f Measure(const Vec2f& offer)const{
		return offer;
	}
	
protected:
	virtual morda::Vec2f ComputeMinDim()const{
		return this->Rect().d;
	}
	
public:

	void SetRelayoutNeeded()noexcept;
	
	void Hide(){
		this->SetHidden(true);
	}

	void Show(){
		this->SetHidden(false);
	}

	void SetHidden(bool hidden){
		this->isHidden = hidden;
		if(this->isHidden){
			this->SetHovered(false);
		}
	}
	
	bool IsHidden()const noexcept{
		return this->isHidden;
	}

	void Enable()noexcept{
		this->SetDisabled(false);
	}

	void Disable()noexcept{
		this->SetDisabled(true);
	}

	void SetDisabled(bool disabled)noexcept{
		this->isDisabled = disabled;
	}
	
	bool IsDisabled()const noexcept{
		return this->isDisabled;
	}
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool Contains(const morda::Vec2f& pos)const noexcept{
		return morda::Rect2f(morda::Vec2f(0, 0), this->Rect().d).Overlaps(pos);
	}
	
protected:
};



}//~namespace
