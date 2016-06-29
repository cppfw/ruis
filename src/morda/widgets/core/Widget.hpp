/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <string>
#include <set>
#include <memory>

#include <utki/Shared.hpp>

#include "../../util/keycodes.hpp"

#include <kolme/Matrix4.hpp>
#include <kolme/Vector2.hpp>
#include <kolme/Rectangle.hpp>

#include "../../render/Texture2D.hpp"

#include "../../config.hpp"

#include <stob/dom.hpp>


namespace morda{



class Container;



class Widget : virtual public utki::Shared{
	friend class Container;
	friend class App;
	
public:
	typedef std::list<std::shared_ptr<Widget>> T_ChildrenList;
	
public:
	class LayoutParams : public utki::Unique{
	public:
		/**
		 * @brief Requests minimal dimensions of the widget.
		 * The widget will always be given minimal space it needs to properly draw.
		 */
		constexpr static const real Min_d = real(-1);
		
		/**
		 * @brief Requests minimal or bigger dimensions of widget.
		 * The widget will be given at least minimal space it needs to properly draw.
		 */
		constexpr static const real Max_d = real(-2);
		
		/**
		 * @brief Requests widget to be same size as its parent.
		 * Minimal size of the widget is assumed to be 0.
		 */
		constexpr static const real Fill_d = real(-3);
		
		LayoutParams(Vec2r dim = Vec2r(Min_d)) :
				dim(dim)
		{}
		
		Vec2r dim;

	public:
		LayoutParams(const stob::Node* chain = nullptr);
		
		virtual ~LayoutParams()noexcept{}
	};

private:
	Container* parentContainer = nullptr;
	T_ChildrenList::iterator parentIter;
	
	std::set<unsigned> hovered;

	bool isVisible_var;

	bool isEnabled_var;

	morda::Rectr rectangle;
	
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

	void renderFromCache(const kolme::Matr4f& matrix)const;
	
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

	const morda::Rectr& rect()const noexcept{
		return this->rectangle;
	}
	
	kolme::Recti computeViewportRect(const Matr4r& matrix)const noexcept;
	
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
	Widget(const stob::Node* chain);//NOTE: no default nullptr to force initializing Widget when it is virtually inherited
	
public:

	virtual ~Widget()noexcept{}

	virtual void render(const morda::Matr4r& matrix)const{}
	
private:
	void renderInternal(const morda::Matr4r& matrix)const;
	
private:
	void onKeyInternal(bool isDown, EKey keyCode);
	
private:
	bool isFocused_v = false;
public:
	
	//return true to consume
	virtual bool onKey(bool isDown, morda::EKey keyCode){
		return false;
	}
	
	void focus()noexcept;
	
	void unfocus()noexcept;
	
	bool isFocused()const noexcept{
		return this->isFocused_v;
	}
	
	/**
	 * @brief Called when keyboard input focus changes.
     */
	virtual void onFocusChanged(){}
	
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
	
	/**
	 * @brief Measure how big a widget wants to be.
	 * Given the given space determine what dimensions widget wants to have to properly draw.
	 * @param quotum - space available to widget. If value is negative then a minimum size needed for proper widget draw possible is assumed.
	 * @return Measured desired widget dimensions.
	 */
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
	
	bool isInteractive()const noexcept{
		return this->isEnabled() && this->isVisible();
	}
	
	/**
	 * @brief Check if point is within the widget bounds.
     * @param pos - point to check in widget coordinates.
     * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
     */
	bool contains(const morda::Vec2r& pos)const noexcept{
		return morda::Rectr(morda::Vec2r(0, 0), this->rect().d).overlaps(pos);
	}
	
	
	virtual void onTopmostChanged(){}
	
	bool isTopmost()const noexcept;
	
	void makeTopmost();
	
	
	template <class T> T* findAncestor(const char* name = nullptr); //defined in Container.hpp
	
	
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


//Widget depends on Container, so need to define it also in all cases when widget is used.
#include "container/Container.hpp"
