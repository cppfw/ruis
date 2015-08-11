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

#include <string>
#include <set>
#include <memory>

#include <ting/Shared.hpp>
#include <ting/util.hpp>

#include "../util/keycodes.hpp"

#include "../util/Matrix4.hpp"
#include "../util/Vector2.hpp"
#include "../util/Rectangle2.hpp"

#include "../render/Texture2D.hpp"

#include "../config.hpp"

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public ting::Shared{
	friend class Container;
	friend class App;
	
public:
	typedef std::list<std::shared_ptr<Widget>> T_ChildrenList;
	
public:
	class LayoutParams{
	public:
		constexpr static const real D_Min = real(-1);
		constexpr static const real D_Max = real(-2);
		
		LayoutParams(Vec2r dim = Vec2r(D_Min)) :
				dim(dim)
		{}
		
		LayoutParams(const stob::Node* chain);
		
		Vec2r dim;

	public:
		static std::unique_ptr<LayoutParams> New(const stob::Node* chain = nullptr){
			return std::unique_ptr<LayoutParams>(new LayoutParams(chain));
		}
		
		virtual ~LayoutParams()noexcept{}
	};

private:
	Container* parent = nullptr;
	T_ChildrenList::iterator parentIter;
	
	std::set<unsigned> hovered;

	bool isVisible;

	bool isEnabled;

	morda::Rect2r rect;
	
	//clip widgets contents by widget's border if set to true
	bool clip;
public:
	bool Clip()const noexcept{
		return this->clip;
	}
	
	void SetClip(bool clip)noexcept{
		this->clip = clip;
	}
	
	
private:
	bool cache;
	mutable Texture2D cacheTex;

	void renderFromCache(const Matr4f& matrix)const;
	
protected:
	void clearCache();
	
public:
	Texture2D renderToTexture(Texture2D&& reuse = Texture2D())const;
	
private:
	//logical ID of the widget
	std::string name_var;
	
	bool relayoutNeeded = true;
	
	std::unique_ptr<stob::Node> layout;
	
	mutable std::unique_ptr<LayoutParams> layoutParams;
public:
	std::unique_ptr<LayoutParams> ResetLayoutParams(std::unique_ptr<LayoutParams> params = nullptr)noexcept;
	
	bool NeedsRelayout()const noexcept{
		return this->relayoutNeeded;
	}
	
	const std::string& name()const noexcept{
		return this->name_var;
	}
	
	const Container* Parent()const noexcept{
		return this->parent;
	}
	
	Container* Parent()noexcept{
		return this->parent;
	}
	
	std::shared_ptr<Widget> RemoveFromParent();
	
	/**
	 * @brief Check if widget is hovered by any pointer.
     * @return true if hovered by any pointer.
	 * @return false otherwise.
     */
	bool IsHovered()const noexcept{
		return this->hovered.size() != 0;
	}
	
	/**
	 * @brief Check if widget is hovered by given pointer.
     * @param pointerID - pointer ID to check against.
     * @return true if widget is hovered by given pointer ID.
	 * @return false otherwise.
     */
	bool IsHovered(unsigned pointerID)const noexcept{
		return this->hovered.find(pointerID) != this->hovered.end();
	}
	
private:
	void SetHovered(bool isHovered, unsigned pointerID){
		if(isHovered){
			if(this->IsHovered(pointerID)){
				return;
			}
			
			this->hovered.insert(pointerID);
		}else{
			if(!this->IsHovered(pointerID)){
				return;
			}
			
			this->hovered.erase(pointerID);
		}
		
		this->OnHoverChanged(pointerID);
	}
	
	void SetUnhovered(){
		this->hovered.clear();
	}
public:

	const morda::Rect2r& Rect()const noexcept{
		return this->rect;
	}
	
	morda::Rect2i ComputeViewportRect(const Matr4r& matrix)const noexcept;
	
	void MoveTo(const morda::Vec2r& newPos)noexcept{
		this->rect.p = newPos;
	}
	
	void MoveBy(const morda::Vec2r& delta)noexcept{
		this->rect.p += delta;
	}

	void Resize(const morda::Vec2r& newDims);
	
	void ResizeBy(const morda::Vec2r& delta){
		this->Resize(this->Rect().d + delta);
	}

	virtual std::shared_ptr<Widget> FindChildByName(const std::string& name)noexcept;
	
	template <typename T> std::shared_ptr<T> FindChildByNameAs(const std::string& name)noexcept{
		return std::dynamic_pointer_cast<T>(this->FindChildByName(name));
	}
	
public:
	Widget(const stob::Node* chain);
	
public:

	virtual ~Widget()noexcept{}

	virtual void render(const morda::Matr4r& matrix)const{}
	
private:
	void renderInternal(const morda::Matr4r& matrix)const;
	
private:
	void OnKeyInternal(bool isDown, EKey keyCode);
	
private:
	bool isFocused = false;
public:
	
	//return true to consume
	virtual bool onKey(bool isDown, morda::EKey keyCode){
		return false;
	}
	
	void Focus()noexcept;
	
	void Unfocus()noexcept;
	
	bool IsFocused()const noexcept{
		return this->isFocused;
	}
	
	/**
	 * @brief Called when keyboard input focus changes.
     */
	virtual void OnFocusedChanged(){}
	
	enum class EMouseButton{
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN,
		
		ENUM_SIZE
	};

	
	
	//return true to consume event
	virtual bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID){
		return false;
	}
	
	//return true to consume event
	virtual bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerID){
		return false;
	}

	virtual void OnHoverChanged(unsigned pointerID){
//		TRACE(<< "Widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	virtual void OnResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
		this->layOut();
	}
	
	/**
	 * @brief Called when parent of this widget changes.
	 * Called when parent of the widget changes. This happens when widget is
	 * added to or removed from a Container.
     */
	virtual void onParentChanged(){}
	
	virtual morda::Vec2r onMeasure(const morda::Vec2r& quotum)const;
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const{
		return this->onMeasure(quotum);
	}
	
public:

	void SetRelayoutNeeded()noexcept;

	virtual void layOut(){}
	
	void SetVisible(bool visible){
		this->isVisible = visible;
		if(!this->isVisible){
			this->SetUnhovered();
		}
	}
	
	bool IsVisible()const noexcept{
		return this->isVisible;
	}

	void SetEnabled(bool enable)noexcept{
		this->isEnabled = enable;
	}
	
	bool IsEnabled()const noexcept{
		return this->isEnabled;
	}
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool Contains(const morda::Vec2r& pos)const noexcept{
		return morda::Rect2r(morda::Vec2r(0, 0), this->Rect().d).Overlaps(pos);
	}
	
	
	virtual void OnTopmostChanged(){}
	
	bool IsTopmost()const noexcept;
	
	void makeTopmost();
	
	
	template <class T> T* findParent(const char* name = nullptr); //defined in Container.hpp
};



}//~namespace
