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
	Container* parentContainer = nullptr;
	T_ChildrenList::iterator parentIter;
	
	std::set<unsigned> hovered;

	bool isVisible_var;

	bool isEnabled_var;

	morda::Rect2r rectangle;
	
	//clip widgets contents by widget's border if set to true
	bool clip_var;
public:
	bool clip()const noexcept{
		return this->clip_var;
	}
	
	void setClip(bool clip)noexcept{
		this->clip_var = clip;
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
	std::string nameOfWidget;
	
	bool relayoutNeeded = true;
	
	std::unique_ptr<stob::Node> layout;
	
	mutable std::unique_ptr<LayoutParams> layoutParams;
public:
	std::unique_ptr<LayoutParams> resetLayoutParams(std::unique_ptr<LayoutParams> params = nullptr)noexcept; //TODO: ?
	
	bool needsRelayout()const noexcept{
		return this->relayoutNeeded;
	}
	
	const std::string& name()const noexcept{
		return this->nameOfWidget;
	}
	
	const Container* parent()const noexcept{
		return this->parentContainer;
	}
	
	Container* parent()noexcept{
		return this->parentContainer;
	}
	
	std::shared_ptr<Widget> removeFromParent();
	
	/**
	 * @brief Check if widget is hovered by any pointer.
     * @return true if hovered by any pointer.
	 * @return false otherwise.
     */
	bool isHovered()const noexcept{
		return this->hovered.size() != 0;
	}
	
	/**
	 * @brief Check if widget is hovered by given pointer.
     * @param pointerID - pointer ID to check against.
     * @return true if widget is hovered by given pointer ID.
	 * @return false otherwise.
     */
	bool isHovered(unsigned pointerID)const noexcept{
		return this->hovered.find(pointerID) != this->hovered.end();
	}
	
private:
	void setHovered(bool isHovered, unsigned pointerID){
		if(isHovered){
			if(this->isHovered(pointerID)){
				return;
			}
			
			this->hovered.insert(pointerID);
		}else{
			if(!this->isHovered(pointerID)){
				return;
			}
			
			this->hovered.erase(pointerID);
		}
		
		this->onHoverChanged(pointerID);
	}
	
	void setUnhovered(){
		this->hovered.clear();
	}
public:

	const morda::Rect2r& rect()const noexcept{
		return this->rectangle;
	}
	
	morda::Rect2i ComputeViewportRect(const Matr4r& matrix)const noexcept;
	
	void moveTo(const morda::Vec2r& newPos)noexcept{
		this->rectangle.p = newPos;
	}
	
	void moveBy(const morda::Vec2r& delta)noexcept{
		this->rectangle.p += delta;
	}

	void resize(const morda::Vec2r& newDims);
	
	void resizeBy(const morda::Vec2r& delta){
		this->resize(this->rect().d + delta);
	}

	virtual std::shared_ptr<Widget> findChildByName(const std::string& name)noexcept;
	
	template <typename T> std::shared_ptr<T> findChildByNameAs(const std::string& name)noexcept{
		return std::dynamic_pointer_cast<T>(this->findChildByName(name));
	}
	
public:
	Widget(const stob::Node* chain);
	
public:

	virtual ~Widget()noexcept{}

	virtual void render(const morda::Matr4r& matrix)const{}
	
private:
	void renderInternal(const morda::Matr4r& matrix)const;
	
private:
	void onKeyInternal(bool isDown, EKey keyCode);
	
private:
	bool isFocused_var = false;
public:
	
	//return true to consume
	virtual bool onKey(bool isDown, morda::EKey keyCode){
		return false;
	}
	
	void focus()noexcept;
	
	void unfocus()noexcept;
	
	bool isFocused()const noexcept{
		return this->isFocused_var;
	}
	
	/**
	 * @brief Called when keyboard input focus changes.
     */
	virtual void onFocusedChanged(){}
	
	enum class EMouseButton{
		LEFT,
		RIGHT,
		MIDDLE,
		WHEEL_UP,
		WHEEL_DOWN,
		
		ENUM_SIZE
	};

	
	
	//return true to consume event
	virtual bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID){
		return false;
	}
	
	//return true to consume event
	virtual bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID){
		return false;
	}

	virtual void onHoverChanged(unsigned pointerID){
//		TRACE(<< "Widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	virtual void onResize(){
//		TRACE(<< "Widget::OnResize(): invoked" << std::endl)
		this->layOut();
	}
	
	/**
	 * @brief Called when parent of this widget changes.
	 * Called when parent of the widget changes. This happens when widget is
	 * added to or removed from a Container.
     */
	virtual void onParentChanged(){}
	
	virtual morda::Vec2r measure(const morda::Vec2r& quotum)const;
	
public:

	void setRelayoutNeeded()noexcept;

	virtual void layOut(){}
	
	void setVisible(bool visible){
		this->isVisible_var = visible;
		if(!this->isVisible_var){
			this->setUnhovered();
		}
	}
	
	bool isVisible()const noexcept{
		return this->isVisible_var;
	}

	void setEnabled(bool enable)noexcept{
		this->isEnabled_var = enable;
	}
	
	bool isEnabled()const noexcept{
		return this->isEnabled_var;
	}
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool contains(const morda::Vec2r& pos)const noexcept{
		return morda::Rect2r(morda::Vec2r(0, 0), this->rect().d).Overlaps(pos);
	}
	
	
	virtual void onTopmostChanged(){}
	
	bool isTopmost()const noexcept;
	
	void makeTopmost();
	
	
	template <class T> T* findParent(const char* name = nullptr); //defined in Container.hpp
	
	
	/**
	 * @brief Calculate position in parent coordinates.
     * @param pos - position to translate to parent coordinates.
     * @param parent - parent of the widget hierarchy relatively to which the translation is to be done.
	 *        If null then it will go down till the root widget.
     * @return translated position.
     */
	Vec2r calcPosInParent(Vec2r pos, const Widget* parent = nullptr);
};



}//~namespace
