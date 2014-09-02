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
#include <ting/util.hpp>

#include "../util/keycodes.hpp"

#include "../util/Matrix4.hpp"
#include "../util/Vector2.hpp"
#include "../util/Rectangle2.hpp"
#include "../util/Background.hpp"

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
	
	bool isHovered = false;

	bool isHidden = false;

	bool isDisabled = false;

	morda::Rect2r rect;
	
	//minimal dimensions needed to show widget's contents normally
	morda::Vec2r minDim;
	bool minDimNeedsRecomputing = true;
	
	//clip widgets contents by widget's border if set to true
	bool clip = false;
public:
	bool IsClip()const NOEXCEPT{
		return this->clip;
	}
	
	void SetClip(bool clip)NOEXCEPT{
		this->clip = clip;
	}
	
private:
	//logical ID of the widget
	std::string name;
	
	bool relayoutNeeded = true;
	
	//properties from STOB description
	std::unique_ptr<stob::Node> prop;
public:
	const stob::Node* Prop()const NOEXCEPT{
		return this->prop.operator->();
	}
	
	const stob::Node* GetPropertyNode(const char* propName)const NOEXCEPT{
		if(!this->Prop()){
			return nullptr;
		}
		return this->Prop()->Child(propName).node();
	}
	
	std::unique_ptr<stob::Node> ExtractProp()NOEXCEPT{
		return std::move(this->prop);
	}
	
	void SetProp(std::unique_ptr<stob::Node> prop)NOEXCEPT{
		this->prop = std::move(prop);
	}
	
	bool NeedsRelayout()const NOEXCEPT{
		return this->relayoutNeeded;
	}
	
	const std::string& Name()const NOEXCEPT{
		return this->name;
	}
	
	const Container* Parent()const NOEXCEPT{
		return this->parent;
	}
	
	Container* Parent()NOEXCEPT{
		return this->parent;
	}
	
	//NOTE: if only parent holds Ref then object may be deleted
	void RemoveFromParent();
	
	bool IsHovered()const NOEXCEPT{
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

	const morda::Rect2r& Rect()const NOEXCEPT{
		return this->rect;
	}
	
	void MoveTo(const morda::Vec2r& newPos)NOEXCEPT{
		this->rect.p = newPos;
	}
	
	void MoveBy(const morda::Vec2r& delta)NOEXCEPT{
		this->rect.p += delta;
	}

	void Resize(const morda::Vec2r& newDims){
		if(this->rect.d == newDims){
			return;
		}
		
		this->rect.d = newDims;
		ting::util::ClampBottom(this->rect.d.x, real(0.0f));
		ting::util::ClampBottom(this->rect.d.y, real(0.0f));
		this->OnResize();//call virtual method
		this->relayoutNeeded = false;
	}
	
	void ResizeBy(const morda::Vec2r& delta){
		this->Resize(this->Rect().d + delta);
	}

	virtual std::shared_ptr<Widget> FindChildByName(const std::string& name)NOEXCEPT{
		return nullptr;
	}
	
public:
	Widget(const stob::Node* desc);
	
public:

	virtual ~Widget()NOEXCEPT{}

	virtual void Render(const morda::Matr4r& matrix)const{}
	
private:
	void RenderInternal(const morda::Matr4r& matrix)const;
	
private:
	void OnKeyInternal(bool isDown, EKey keyCode);
	
private:
	bool isFocused = false;
public:
	
	//return true to consume
	virtual bool OnKey(bool isDown, morda::EKey keyCode){
		return false;
	}
	
	void Focus()NOEXCEPT;
	
	void Unfocus()NOEXCEPT;
	
	bool IsFocused()const NOEXCEPT{
		return this->isFocused;
	}
	
	virtual void OnFocusedChanged(){}
	
	enum class EMouseButton{
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN,
		
		ENUM_SIZE
	};

	std::function<bool (Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)> onMouseButton;
	
	//return true to consume event
	virtual bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
		if(this->onMouseButton){
			return this->onMouseButton(*this, isDown, pos, button, pointerId);
		}
		return false;
	}

	std::function<bool (Widget& widget, const morda::Vec2r& pos, unsigned pointerId)> onMouseMove;
	
	//return true to consume event
	virtual bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerId){
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
	
	const morda::Vec2r& GetMinDim()const{
		if(this->minDimNeedsRecomputing){
			const_cast<Widget*>(this)->minDim = this->ComputeMinDim();
			const_cast<std::remove_const<std::remove_pointer<decltype(this)>::type>::type*>(this)->minDimNeedsRecomputing = false;
		}
		return this->minDim;
	}

	virtual morda::Vec2r Measure(const Vec2r& offer)const{
		return offer;
	}
	
protected:
	virtual morda::Vec2r ComputeMinDim()const{
		return morda::Vec2r(0, 0);
	}
	
public:

	void SetRelayoutNeeded()NOEXCEPT;
	
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
	
	bool IsHidden()const NOEXCEPT{
		return this->isHidden;
	}

	void Enable()NOEXCEPT{
		this->SetDisabled(false);
	}

	void Disable()NOEXCEPT{
		this->SetDisabled(true);
	}

	void SetDisabled(bool disabled)NOEXCEPT{
		this->isDisabled = disabled;
	}
	
	bool IsDisabled()const NOEXCEPT{
		return this->isDisabled;
	}
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool Contains(const morda::Vec2r& pos)const NOEXCEPT{
		return morda::Rect2r(morda::Vec2r(0, 0), this->Rect().d).Overlaps(pos);
	}
	
private:
	
	std::unique_ptr<Background> background;
	
public:
	void SetBackground(std::unique_ptr<Background> b){
		this->background = std::move(b);
	}
};



}//~namespace
