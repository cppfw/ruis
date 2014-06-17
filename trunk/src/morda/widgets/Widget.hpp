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
#include <ting/Ref.hpp>

#include "../util/keycodes.hpp"

#include "../util/Matrix4.hpp"
#include "../util/Vector2.hpp"
#include "../util/Rectangle2.hpp"

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public ting::RefCounted{
	friend class Container;
	friend class App;
	
	ting::Ref<Widget> next, prev;//used to store widgets in Container as double-linked list
	
private:
	ting::WeakRef<Container> parent;
	
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
	bool GetClip()const throw(){
		return this->clip;
	}
	
	void SetClip(bool clip)throw(){
		this->clip = clip;
	}
	
private:
	//logical ID of the widget
	std::string name;
	
	ting::Inited<bool, true> relayoutNeeded;
	
	//properties from STOB description
	ting::Ptr<stob::Node> prop;
public:
	inline const stob::Node* Prop()const throw(){
		return this->prop.operator->();
	}
	
	//TODO: rename method to avoid confusion with Node::GetProperty()
	const stob::Node* GetProperty(const char* propName)const throw(){
		if(!this->Prop()){
			return 0;
		}
		return this->Prop()->Child(propName).node();
	}
	
	inline ting::Ptr<stob::Node> ExtractProp()throw(){
		return this->prop;
	}
	
	inline void SetProp(ting::Ptr<stob::Node> prop)throw(){
		this->prop = prop;
	}
	
	inline bool NeedsRelayout()const throw(){
		return this->relayoutNeeded;
	}
	
	const std::string& Name()const throw(){
		return this->name;
	}
	
	inline const ting::WeakRef<Container>& Parent()throw(){
		return this->parent;
	}
	
	inline const ting::WeakRef<const Container>& Parent()const throw(){
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
	
private:
	void SetHovered(bool isHovered){
		if(this->isHovered == isHovered){
			return;
		}
		
		this->isHovered = isHovered;
		this->OnHoverChanged();
	}
	
public:

	inline const morda::Rect2f& Rect()const throw(){
		return this->rect;
	}
	
	inline void MoveTo(const morda::Vec2f& newPos)throw(){
		this->rect.p = newPos;
	}
	
	inline void MoveBy(const morda::Vec2f& delta)throw(){
		this->rect.p += delta;
	}

	inline void Resize(const morda::Vec2f& newDims){
		this->rect.d = newDims;
		this->OnResize();//call virtual method
		this->relayoutNeeded = false;
	}

	virtual ting::Ref<Widget> FindChildByName(const std::string& name)throw(){
		return 0;
	}
	
protected:
	//NOTE: dummy int argument to avoid implicit call of default constructor in derived classes, to avoid silent bugs.
	Widget(int dummy){}
	
	Widget(const stob::Node& description);
	
public:
	static ting::Ref<Widget> New(const stob::Node& description){
		return ting::Ref<Widget>(new Widget(description));
	}
	
	static ting::Ref<Widget> New(){
		return ting::Ref<Widget>(new Widget(0));
	}

	virtual ~Widget()throw(){}

	virtual void Render(const morda::Matr4f& matrix)const{};
	
private:
	void RenderInternal(const morda::Matr4f& matrix)const;
	
private:
	void OnKeyInternal(bool isDown, key::Key keyCode);
	
private:
	ting::Inited<bool, false> isFocused;
public:
	
	//return true to consume
	virtual bool OnKey(bool isDown, key::Key keyCode){
		return false;
	}
	
	void Focus()throw();
	
	void Unfocus()throw();
	
	inline bool IsFocused()const throw(){
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

	//return true to consume event
	virtual bool OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
		return false;
	}

	//return true to consume event
	virtual bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
		return false;
	}

	virtual void OnHoverChanged(){
//		TRACE(<< "Widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	virtual void OnResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
	}
	
	inline const morda::Vec2f& GetMinDim()const{
		if(this->minDimNeedsRecomputing){
			const_cast<Widget*>(this)->minDim = this->ComputeMinDim();
			this->minDimNeedsRecomputing = false;
		}
		return this->minDim;
	}

protected:
	virtual morda::Vec2f ComputeMinDim()const{
		return this->Rect().d;
	}
	
public:

	void SetRelayoutNeeded()throw();
	
	inline void Hide(){
		this->SetHidden(true);
	}

	inline void Show(){
		this->SetHidden(false);
	}

	inline void SetHidden(bool hidden){
		this->isHidden = hidden;
		if(this->isHidden){
			this->SetHovered(false);
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
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool Contains(const morda::Vec2f& pos)const throw(){
		return morda::Rect2f(morda::Vec2f(0, 0), this->Rect().d).Overlaps(pos);
	}
	
protected:
};



}//~namespace
