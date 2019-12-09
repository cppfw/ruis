#pragma once

#include <string>
#include <set>
#include <memory>

#include <utki/Shared.hpp>
#include <utki/exception.hpp>

#include <puu/dom.hpp>

#include <r4/matrix4.hpp>
#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include "../config.hpp"

#include "../render/Texture2D.hpp"

#include "../util/keycodes.hpp"
#include "../util/MouseButton.hpp"

#include "../exception.hpp"


namespace morda{



class container;


/**
 * @brief Basic widget class.
 * From GUI script it can be instantiated as "Widget".
 * It can have the following parameters:
 * @param layout - layout parameters description. See widget::LayoutParams for details.
 * @param x - horizontal position within parent widget.
 * @param y - vertical position within parent widget.
 * @param dx - width of the widget.
 * @param dy - height of the widget.
 * @param id - id assigned to widget.
 * @param clip - enable (true) or disable (false) the scissor test for this widget boundaries when rendering. Default value is false.
 * @param cache - enable (true) or disable (false) pre-rendering this widget to texture and render from texture for faster rendering.
 * @param visible - should the widget be initially visible (true) or hidden (false). Default value is true.
 * @param enabled - should the widget be initially enabled (true) or disabled (false). Default value is true. Disabled widgets do not get any input from keyboard or mouse.
 */
class widget : virtual public utki::Shared{
	friend class container;
	friend class Morda;
public:
	/**
	 * @brief Basic layout parameters.
	 */
	class LayoutParams : public utki::Unique{
	public:
		/**
		 * @brief Requests minimal dimensions of the widget.
		 * The widget will always be given minimal space it needs to properly draw.
		 */
		constexpr static const real min_c = real(-1);

		/**
		 * @brief Requests minimal or bigger dimensions of widget.
		 * The widget will be given at least minimal space it needs to properly draw.
		 */
		constexpr static const real max_c = real(-2);

		/**
		 * @brief Requests widget to be same size as its parent.
		 * Minimal size of the widget is assumed to be 0.
		 */
		constexpr static const real fill_c = real(-3);

		LayoutParams(Vec2r dim = Vec2r(min_c)) :
				dim(dim)
		{}

		/**
		 * @brief desired dimensions.
		 * Components should hold positive value in pixels or min_c, max_c, fill_c.
		 */
		Vec2r dim;

	public:
		LayoutParams(const stob::Node* chain = nullptr);

		virtual ~LayoutParams()noexcept{}
	};

private:
	container* parent_v = nullptr;

	std::set<unsigned> hovered;

	bool isVisible_v;

	bool isEnabled_v;

	morda::Rectr rectangle;

	//clip widgets contents by widget's border if set to true
	bool clip_v;
public:
	/**
	 * @brief Check if scissor test is enabled for this widget.
	 * @return true if scissor test is enabled.
	 * @return false otherwise.
	 */
	bool is_clip_enabled()const noexcept{
		return this->clip_v;
	}

	/**
	 * @brief Enable/Disable scissor test.
	 * @param enable - whether to enable (true) or disable (false) the scissor test.
	 */
	void set_clip(bool enable)noexcept{
		this->clip_v = enable;
	}


private:
	bool cache;
	mutable bool cacheDirty = true;
	mutable std::shared_ptr<Texture2D> cacheTex;

	void renderFromCache(const r4::mat4f& matrix)const;

protected:
	void clearCache();

public:
	/**
	 * @brief Enable/disable caching.
	 * If caching is enabled for this widget then it will first be rendered to a texture.
	 * And then the texture will be rendered to the frame buffer each time the widget needs to be drawn.
	 * Next time the texture will be re-rendered only when cache is cleared.
	 * @param enabled - whether to enable or disable the caching.
	 */
	void setCache(bool enabled)noexcept{
		this->cache = enabled;
	}

	/**
	 * @brief Render this widget to texture.
	 * @param reuse - try to re-use the existing texture to avoid new texture allocation.
	 *                If dimensions of the passed in texture are different from dimensions
	 *                of the widget then new texture will be allocated.
	 * @return Texture containing rendered widget.
	 */
	std::shared_ptr<Texture2D> renderToTexture(std::shared_ptr<Texture2D> reuse = nullptr)const;

private:
	bool relayoutNeeded = true;

	std::unique_ptr<stob::Node> layout;

	mutable std::unique_ptr<LayoutParams> layoutParams;
public:
	std::string id;

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 * @throw morda::Exc if the widget is not added to any container.
	 */
	LayoutParams& getLayoutParams();

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 * @throw morda::Exc if the widget is not added to any container.
	 */
	const LayoutParams& getLayoutParams()const;

	/**
	 * @brief Check if this widget's contents need to be layed out.
	 * @return true if this widget needs re-layout.
	 * @return false otherwise.
	 */
	bool needsRelayout()const noexcept{
		return this->relayoutNeeded;
	}

	/**
	 * @brief Get name of the widget.
	 * @return Name of the widget.
	 */
	//TODO: remove deprecated code
	const std::string& name()const noexcept{
		TRACE(<< "DEPRECATED!!! name is deprecated, use id" << std::endl)
		return this->id;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	const container* parent()const noexcept{
		return this->parent_v;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	container* parent()noexcept{
		return this->parent_v;
	}

	/**
	 * @brief Remove widget from parent container.
	 * Be careful when calling this method from within another method of the widget
	 * as it may result in immediate destruction of the widget if there are no
	 * pointers to this widget left.
	 * @return pointer to this widget.
	 */
	std::shared_ptr<widget> removeFromParent();

	/**
	 * @brief Replace this widget by another widget.
	 * Replaces this widget in its parent by the given widget.
	 * @param w - widget to replace this one by.
	 * @return Shared pointer to this widget.
	 */
	std::shared_ptr<widget> replaceBy(std::shared_ptr<widget> w);

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
	void setHovered(bool isHovered, unsigned pointerID);

	void setUnhovered();
public:

	/**
	 * @brief Get widget's rectangle.
	 * The rectangle is in parent's coordinates.
	 * @return Widget's rectangle.
	 */
	const morda::Rectr& rect()const noexcept{
		return this->rectangle;
	}

	/**
	 * @brief Get rectangle occupied by the widget in viewport coordinates.
	 * @param matrix - transformation matrix which transforms point (0,0) to left top corner point of the widget.
	 * @return Rectangle of the widget in viewport coordinates.
	 */
	r4::recti computeViewportRect(const Matr4r& matrix)const noexcept;

	/**
	 * @brief Move widget to position within its parent.
	 * @param newPos - new widget's position.
	 */
	void moveTo(const morda::Vec2r& newPos)noexcept{
		this->rectangle.p = newPos;
	}

	/**
	 * @brief Shift widget within its parent.
	 * @param delta - vector to shift the widget by.
	 */
	void moveBy(const morda::Vec2r& delta)noexcept{
		this->rectangle.p += delta;
	}

	/**
	 * @brief Set new dimensions of the widget.
	 * @param newDims - new dimensions of the widget.
	 */
	void resize(const morda::Vec2r& newDims);

	/**
	 * @brief Extend or shrink the widget dimensions by given value.
	 * @param delta - value to change the widget dimensions by.
	 */
	void resizeBy(const morda::Vec2r& delta){
		this->resize(this->rect().d + delta);
	}

	/**
	 * @brief Try get widget by id.
	 * Get widget by ID from the widget hierarchy. Since simple Widget cannot have children,
	 * this implementation just checks if id of this widget is the looked up id and if so it returns
	 * pointer to this widget or nullptr otherwise.
	 * @param id - id of the widget to search for.
	 * @return pointer to the widget if found.
	 * @return nullptr if there is no widget with given id found.
	 */
	virtual std::shared_ptr<widget> try_get_widget(const std::string& id)noexcept;

	//TODO: remove deprecated code
	std::shared_ptr<widget> findById(const std::string& id)noexcept{
		TRACE(<< "DEPRECATED!!! findById() is deprecated, use try_get_widget()" << std::endl)
		return this->try_get_widget(id);
	}

	//TODO: remove deprecated code
	std::shared_ptr<widget> findByName(const std::string& name)noexcept{
		TRACE(<< "DEPRECATED!!! findByName() is deprecated, use try_get_widget()" << std::endl)
		return this->try_get_widget(name);
	}

	/**
	 * @brief Try get widget by id.
	 * Same as try_get_widget() but also tries to cast the widget object to a specified class.
	 * @param id - id of the widget to search for.
	 * @return pointer to the widget if found.
	 * @return nullptr if there is no widget with given id found or if the widget could not be cast to specified class.
	 */
	template <typename T> std::shared_ptr<T> try_get_widget_as(const std::string& id)noexcept{
		return std::dynamic_pointer_cast<T>(this->try_get_widget(id));
	}

	//TODO: remove deprecated code
	template <typename T> std::shared_ptr<T> findByIdAs(const std::string& id)noexcept{
		TRACE(<< "DEPRECATED!!! findByIdAs() is deprecated, use try_get_widget_as()" << std::endl)
		return this->try_get_widget_as<T>(id);
	}

	//TODO: remove deprecated code
	template <typename T> std::shared_ptr<T> findByNameAs(const std::string& name)noexcept{
		TRACE(<< "DEPRECATED!!! findByNameAs() is deprecated, use try_get_widget_as()" << std::endl)
		return this->try_get_widget_as<T>(name);
	}

	//TODO: deprecated, remove.
	/**
	 * @brief Child widget with requested ID is not found within the parent container.
	 */
	typedef utki::not_found WidgetNotFoundExc;

	/**
	 * @brief Get widget.
	 * @param id - id of the widget to get.
	 * @return reference to the widget.
	 * @throw utki::not_found - if no widget with given id has been found.
	 */
	widget& get_widget(const std::string& id);

	//TODO: remove deprecated code
	widget& getById(const std::string& id){
		TRACE(<< "DEPRECATED!!! getByName() is deprecated, use get_widget()" << std::endl)
		return this->get_widget(id);
	}

	//TODO: remove deprecated code
	widget& getByName(const std::string& name){
		TRACE(<< "DEPRECATED!!! getByName() is deprecated, use get_widget()" << std::endl)
		return this->get_widget(name);
	}

	/**
	 * @brief Get widget of specific type by its id.
	 * @param id - id of the widget to get.
	 * @return reference to the requested widget.
	 * @throw utki::not_found - if no widget with given id has been found.
	 * @throw std::bad_cast - if requested widget is not of the specified type.
	 */
	template <typename T> T& get_widget_as(const std::string& id){
		return dynamic_cast<T&>(this->get_widget(id));
	}

	//TODO: remove deprecated code
	template <typename T> T& getByIdAs(const std::string& id){
	TRACE(<< "DEPRECATED!!! getByNameAs() is deprecated, use get_widget_as()" << std::endl)
		return this->get_widget_as<T>(id);
	}

	//TODO: remove deprecated code
	template <typename T> T& getByNameAs(const std::string& name){
		TRACE(<< "DEPRECATED!!! getByNameAs() is deprecated, use get_widget_as()" << std::endl)
		return this->get_widget_as<T>(name);
	}

public:
	/**
	 * @brief Constructor.
	 * @param chain - STOB description of the widget.
	 */
	widget(const stob::Node* chain);//NOTE: no default nullptr to force initializing Widget when it is virtually inherited

public:

	virtual ~widget()noexcept{}

	/**
	 * @brief Render widget to screen.
	 * Override this method to render sub-classed widgets. This
	 * implementation does nothing.
	 * @param matrix - transformation matrix to use when rendering.
	 */
	virtual void render(const morda::Matr4r& matrix)const{}

private:
	void renderInternal(const morda::Matr4r& matrix)const;

private:
	void onKeyInternal(bool isDown, Key_e keyCode);

private:
	bool isFocused_v = false;
public:

	/**
	 * @brief Handle keyboard key event.
	 * This method is called by framework when a widget is requested to handle a key event. So, the widget is either a focused widget or root widget.
	 * @param isDown - was the key pressed (true) or released (false).
	 * @param keyCode - keyboard key code.
	 * @return true to consume event and prevent its further propagation.
	 * @return false to allow the event to be propagated further.
	 */
	virtual bool onKey(bool isDown, morda::Key_e keyCode){
		return false;
	}

	/**
	 * @brief Set this widget as focused.
	 * Focused widget will receive keyboard events.
	 */
	void focus()noexcept;

	/**
	 * @brief Unfocus this widget.
	 */
	void unfocus()noexcept;

	/**
	 * @brief Check if this widget is focused.
	 * @return true if widget is focused.
	 * @return false otherwise.
	 */
	bool isFocused()const noexcept{
		return this->isFocused_v;
	}

	/**
	 * @brief Called when keyboard input focus changes.
	 */
	virtual void onFocusChanged(){}


	/**
	 * @brief Handle mouse button event.
	 * This function is called by framework when widget receives mouse button event.
	 * @param isDown - was the button pressed (true) or released (false).
	 * @param pos - position of the mouse cursor at the moment when the button event has occurred, in widget local coordinates.
	 * @param button - mouse button.
	 * @param pointerID - ID of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID){
		return false;
	}

	/**
	 * @brief Handle mouse move event.
	 * Called by framework when mouse pointer was moved within the widget.
	 * @param pos - new position of the mouse cursor in widget local coordinates.
	 * @param pointerID - ID of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID){
		return false;
	}

	/**
	 * @brief Handle mouse hover changed event.
	 * Called by framework when mouse pointer enters or leaves the widget boundaries.
	 * @param pointerID - ID of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 */
	virtual void onHoverChanged(unsigned pointerID){
//		TRACE(<< "widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	/**
	 * @brief Invoked when widget's size changes.
	 * Default implementation performs laying out of the widget by calling its layout() method.
	 */
	virtual void onResize(){
//		TRACE(<< "widget::OnResize(): invoked" << std::endl)
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
	 * @param quotum - space available to widget. If value is negative then a minimum size needed for proper widget drawing is assumed.
	 * @return Measured desired widget dimensions.
	 */
	virtual morda::Vec2r measure(const morda::Vec2r& quotum)const;

public:

	/**
	 * @brief Request re-layout.
	 * Set a flag on the widget indicating to the framework that the widget needs a re-layout.
	 * The layout will be performed before drawing.
	 */
	void setRelayoutNeeded()noexcept;

	/**
	 * @brief Perform layout of the widget.
	 * Override this method to arrange widgets contents if needed.
	 */
	virtual void layOut(){}

	/**
	 * @brief Show/hide widget.
	 * @param visible - whether to show (true) or hide (false) the widget.
	 */
	void setVisible(bool visible);

	/**
	 * @brief Check if widget is visible.
	 * @return true if the widget is visible.
	 * @return false otherwise.
	 */
	bool isVisible()const noexcept{
		return this->isVisible_v;
	}

	/**
	 * @brief Enable/disable the widget.
	 * @param enable - whether to enable (true) or disable (false) the widget.
	 */
	void setEnabled(bool enable);

	/**
	 * @brief Check if the widget is currently enabled.
	 * @return true if the widget is currently enabled.
	 * @return false otherwise.
	 */
	bool isEnabled()const noexcept{
		return this->isEnabled_v;
	}

	/**
	 * @brief Invoked when enabled state of the widget changes.
	 */
	virtual void onEnabledChanged(){}

	/**
	 * @brief Check if widget can receive user input.
	 * Checks if widget is enabled and visible, so it can receive user input.
	 * @return true if widget can receive user input.
	 * @return false otherwise.
	 */
	bool isInteractive()const noexcept{
		return this->isEnabled() && this->isVisible();
	}

	/**
	 * @brief Check if point is within the widget bounds.
	 * @param pos - point to check in widget coordinates.
	 * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
	 */
	bool overlaps(const morda::Vec2r& pos)const noexcept{
		return morda::Rectr(morda::Vec2r(0, 0), this->rect().d).overlaps(pos);
	}

	/**
	 * @brief Find ancestor with given name and of given class.
	 * @param name - name to look for. If nullptr, then any ancestor of the given class will match.
	 * @return pointer to the found ancestor widget.
	 * @return false if no ancestor which satisfies the conditions was found.
	 */
	template <class T> T* findAncestor(const char* name = nullptr); //defined in Container.hpp

	/**
	 * @brief Get position relative to ancestor.
	 * @param ancestor - ancestor widget relative to which to get the position.
	 * @return The position of this widget in given ancestor's coordinates.
	 */
	Vec2r posInAncestor(const widget& ancestor);

	/**
	 * @brief Calculate position in parent coordinates.
	 * @param pos - position to translate to parent coordinates.
	 * @param parent - parent of the widget hierarchy relatively to which the translation is to be done.
	 *        If null then it will go down till the root widget.
	 * @return translated position.
	 */
	Vec2r calcPosInParent(Vec2r pos, const widget* parent = nullptr);
};

//TODO: deprecated, remove.
typedef widget Widget;

}


//Widget depends on Container, so need to define it also in all cases when widget is used.
#include "container.hpp"
