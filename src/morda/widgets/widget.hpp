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
	class layout_params : public utki::Unique{
	public:
		/**
		 * @brief Requests minimal dimensions of the widget.
		 * The widget will always be given minimal space it needs to properly draw.
		 */
		constexpr static const real min = real(-1);

		//TODO: deprecated, remove.
		constexpr static const real min_c = min;

		/**
		 * @brief Requests minimal or bigger dimensions of widget.
		 * The widget will be given at least minimal space it needs to properly draw.
		 */
		constexpr static const real max = real(-2);

		//TODO: deprecated, remove.
		constexpr static const real max_c = max;

		/**
		 * @brief Requests widget to be same size as its parent.
		 * Minimal size of the widget is assumed to be 0.
		 */
		constexpr static const real fill = real(-3);

		//TODO: deprecated, remove.
		constexpr static const real fill_c = fill;

		/**
		 * @param dims - desired dimensions within the layout. Each component can be a non-negative value in pixels or [min, max, fill].
		 */
		layout_params(vector2 dims = vector2(min)) :
				dims(dims)
		{}

		/**
		 * @brief desired dimensions.
		 * Components should hold non-negative value in pixels or [min, max, fill].
		 */
		vector2 dims;

		//TODO: deprecated, remove.
		vector2& dim = dims;

	public:
		layout_params(const stob::Node* chain = nullptr);

		virtual ~layout_params()noexcept{}
	};

	//TODO: deprecated, remove.
	typedef layout_params LayoutParams;

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
	void set_cache(bool enabled)noexcept{
		this->cache = enabled;
	}

	//TODO: deprecated, remove.
	void setCache(bool enabled)noexcept{
		this->set_cache(enabled);
	}

	/**
	 * @brief Render this widget to texture.
	 * @param reuse - try to re-use the existing texture to avoid new texture allocation.
	 *                If dimensions of the passed in texture are different from dimensions
	 *                of the widget then new texture will be allocated.
	 * @return Texture containing rendered widget.
	 */
	std::shared_ptr<Texture2D> render_to_texture(std::shared_ptr<Texture2D> reuse = nullptr)const;

	//TODO: deprecated, remove.
	std::shared_ptr<Texture2D> renderToTexture(std::shared_ptr<Texture2D> reuse = nullptr)const{
		return this->render_to_texture(std::move(reuse));
	}

private:
	bool relayoutNeeded = true;

	std::unique_ptr<stob::Node> layout_description;

	mutable std::unique_ptr<LayoutParams> layoutParams;
public:
	std::string id;

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 * @throw morda::Exc if the widget is not added to any container.
	 */
	layout_params& get_layout_params();

	//TODO: deprecated, remove.
	layout_params& getLayoutParams(){
		return this->get_layout_params();
	}

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 * @throw morda::Exc if the widget is not added to any container.
	 */
	const LayoutParams& get_layout_params()const;

	//TODO: deprecated, remove.
	const LayoutParams& getLayoutParams()const{
		return this->get_layout_params();
	}

	/**
	 * @brief Request re-layout.
	 * Set a flag on the widget indicating to the framework that the widget needs a re-layout.
	 * The layout will be performed before drawing.
	 */
	void invalidate_layout()noexcept;

	//TODO: deprecated, remove.
	void setRelayoutNeeded()noexcept{
		this->invalidate_layout();
	}

	/**
	 * @brief Perform layout of the widget.
	 * Override this method to arrange widgets contents if needed.
	 */
	virtual void lay_out(){
		this->layOut(); // TODO: deprecated, remove.
	}

	//TODO: deprecated, remove.
	virtual void layOut(){}

	/**
	 * @brief Check if this widget's contents need to be layed out.
	 * @return true if this widget needs re-layout.
	 * @return false otherwise.
	 */
	bool is_layout_invalid()const noexcept{
		return this->relayoutNeeded;
	}

	//TODO: deprecated, remove.
	bool needsRelayout()const noexcept{
		return this->is_layout_invalid();
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
	std::shared_ptr<widget> remove_from_parent();

	//TODO: deprecated, remove.
	std::shared_ptr<widget> removeFromParent(){
		return this->remove_from_parent();
	}

	/**
	 * @brief Replace this widget by another widget.
	 * Replaces this widget in its parent by the given widget.
	 * @param w - widget to replace this one by.
	 * @return Shared pointer to this widget.
	 */
	std::shared_ptr<widget> replace_by(std::shared_ptr<widget> w);

	//TODO: deprecaed, remove.
	std::shared_ptr<widget> replaceBy(std::shared_ptr<widget> w){
		return this->replace_by(std::move(w));
	}

	/**
	 * @brief Check if the widget is hovered by at least one pointer.
	 * @return true if hovered by at least one pointer.
	 * @return false otherwise.
	 */
	bool is_hovered()const noexcept{
		return this->hovered.size() != 0;
	}

	//TODO: deprecated, remove.
	bool isHovered()const noexcept{
		return this->is_hovered();
	}

	/**
	 * @brief Check if widget is hovered by given pointer.
	 * @param pointer_id - pointer id to check against.
	 * @return true if widget is hovered by given pointer id.
	 * @return false otherwise.
	 */
	bool is_hovered(unsigned pointer_id)const noexcept{
		return this->hovered.find(pointer_id) != this->hovered.end();
	}

	//TODO: deprecated, remove.
	bool isHovered(unsigned pointerID)const noexcept{
		return this->is_hovered(pointerID);
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
	const morda::rectangle& rect()const noexcept{
		return this->rectangle;
	}

	/**
	 * @brief Get rectangle occupied by the widget in viewport coordinates.
	 * @param matrix - transformation matrix which transforms point (0,0) to left top corner point of the widget.
	 * @return Rectangle of the widget in viewport coordinates.
	 */
	r4::recti compute_viewport_rect(const matrix4& matrix)const noexcept;

	//TODO: deprecated, remove.
	r4::recti computeViewportRect(const Matr4r& matrix)const noexcept{
		return this->compute_viewport_rect(matrix);
	}

	/**
	 * @brief Move widget to position within its parent.
	 * @param newPos - new widget's position.
	 */
	void move_to(const vector2& new_pos)noexcept{
		this->rectangle.p = new_pos;
	}

	//TODO: deprecated, remove.
	void moveTo(const morda::Vec2r& newPos)noexcept{
		this->move_to(newPos);
	}

	/**
	 * @brief Shift widget within its parent.
	 * @param delta - vector to shift the widget by.
	 */
	void move_by(const vector2& delta)noexcept{
		this->rectangle.p += delta;
	}

	//TODO: deprecated, remove.
	void moveBy(const morda::Vec2r& delta)noexcept{
		this->move_by(delta);
	}

	/**
	 * @brief Set new dimensions of the widget.
	 * @param new_dims - new dimensions of the widget.
	 */
	void resize(const vector2& new_dims);

	/**
	 * @brief Extend or shrink the widget dimensions by given value.
	 * @param delta - value to change the widget dimensions by.
	 */
	void resize_by(const vector2& delta){
		this->resize(this->rect().d + delta);
	}

	//TODO: deprecated, remove.
	void resizeBy(const morda::Vec2r& delta){
		this->resize_by(delta);
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
	 * @param is_down - was the key pressed (true) or released (false).
	 * @param key_code - keyboard key code.
	 * @return true to consume event and prevent its further propagation.
	 * @return false to allow the event to be propagated further.
	 */
	virtual bool on_key(bool is_down, morda::Key_e key_code){
		return this->onKey(is_down, key_code); //TODO: deprecated, remove. Should just return false.
	}

	//TODO: deprecated, remove.
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
	bool is_focused()const noexcept{
		return this->isFocused_v;
	}

	bool isFocused()const noexcept{
		return this->is_focused();
	}

	/**
	 * @brief Called when keyboard input focus changes.
	 */
	virtual void on_focus_changed(){}

	/**
	 * @brief Handle mouse button event.
	 * This function is called by framework when widget receives mouse button event.
	 * @param is_down - was the button pressed (true) or released (false).
	 * @param pos - position of the mouse cursor at the moment when the button event has occurred, in widget local coordinates.
	 * @param button - mouse button.
	 * @param pointer_id - id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_button(bool is_down, const morda::Vec2r& pos, MouseButton_e button, unsigned pointer_id){
		return this->onMouseButton(is_down, pos, button, pointer_id); //TODO: deprecated, remove. Should return false.
	}

	//TODO: deprecated, remove.
	virtual bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID){
		return false;
	}

	/**
	 * @brief Handle mouse move event.
	 * Called by framework when mouse pointer was moved within the widget.
	 * @param pos - new position of the mouse cursor in widget local coordinates.
	 * @param pointer_id - id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_move(const morda::Vec2r& pos, unsigned pointer_id){
		return this->onMouseMove(pos, pointer_id); //TODO: deprecated, remove. Should return false.
	}

	//TODO: deprecated, remove.
	virtual bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID){
		return false;
	}

	/**
	 * @brief Handle mouse hover changed event.
	 * Called by framework when mouse pointer enters or leaves the widget boundaries.
	 * @param pointer_id - id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 */
	virtual void on_hover_changed(unsigned pointer_id){
//		TRACE(<< "widget::on_hover_changed(): this->IsHovered() = " << this->IsHovered() << std::endl)
		this->onHoverChanged(pointer_id); //TODO: deprecated, remove.
	}

	//TODO: deprecated, remove.
	virtual void onHoverChanged(unsigned pointerID){
//		TRACE(<< "widget::OnHoverChanged(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	/**
	 * @brief Invoked when widget's size changes.
	 * Default implementation performs laying out of the widget by calling its layout() method.
	 */
	virtual void on_resize(){
//		TRACE(<< "widget::OnResize(): invoked" << std::endl)
		this->onResize(); //TODO: deprecated, remove.
		// this->lay_out(); //TODO: uncomment
	}

	//TODO: deprecated, remove.
	virtual void onResize(){
//		TRACE(<< "widget::OnResize(): invoked" << std::endl)
		this->lay_out();
	}

	/**
	 * @brief Called when parent of this widget changes.
	 * Called when parent of the widget changes. This happens when widget is
	 * added to or removed from a Container.
	 */
	virtual void on_parent_changed(){
		this->onParentChanged(); //TODO: deprecated, remove.
	}

	//TODO: deprecated, remove.
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
	 * @brief Show/hide widget.
	 * @param visible - whether to show (true) or hide (false) the widget.
	 */
	void set_visible(bool visible);

	//TODO: deprecated, remove.
	void setVisible(bool visible){
		this->set_visible(visible);
	}

	/**
	 * @brief Check if widget is visible.
	 * @return true if the widget is visible.
	 * @return false otherwise.
	 */
	bool is_visible()const noexcept{
		return this->isVisible_v;
	}

	//TODO: deprecated, remove.
	bool isVisible()const noexcept{
		return this->is_visible();
	}

	/**
	 * @brief Enable/disable the widget.
	 * @param enable - whether to enable (true) or disable (false) the widget.
	 */
	void set_enabled(bool enable);

	//TODO: deprecated, remove.
	void setEnabled(bool enable){
		this->set_enabled(enable);
	}

	/**
	 * @brief Check if the widget is currently enabled.
	 * @return true if the widget is currently enabled.
	 * @return false otherwise.
	 */
	bool is_enabled()const noexcept{
		return this->isEnabled_v;
	}

	//TODO: deprecated, remove.
	bool isEnabled()const noexcept{
		return this->is_enabled();
	}

	/**
	 * @brief Invoked when enabled state of the widget changes.
	 */
	virtual void on_enabled_changed(){
		this->onEnabledChanged(); //TODO: deprecated, remove.
	}

	//TODO: deprecated, remove.
	virtual void onEnabledChanged(){}

	/**
	 * @brief Check if widget can receive user input.
	 * Checks if widget is enabled and visible, so it can receive user input.
	 * @return true if widget can receive user input.
	 * @return false otherwise.
	 */
	bool is_interactive()const noexcept{
		return this->is_enabled() && this->is_visible();
	}

	//TODO: deprecated, remove.
	bool isInteractive()const noexcept{
		return this->is_interactive();
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
	 * @brief Find ancestor with given id and of given class.
	 * @param id - id to look for. If nullptr, then any ancestor of the given class will match.
	 * @return pointer to the found ancestor widget.
	 * @return false if no ancestor which satisfies the conditions was found.
	 */
	template <class T> T* find_ancestor(const char* id = nullptr); //defined in container.hpp

	//TODO: deprecated, remove.
	template <class T> T* findAncestor(const char* name = nullptr){
		return this->find_ancestor<T>(name);
	}

	/**
	 * @brief Calculate position in ancestor coordinates.
	 * @param pos - position to translate to ancestor coordinates.
	 * @param ancestor - ancestor of the widget hierarchy relatively to which the translation is to be done.
	 *        If null then it will go down till the root widget.
	 * @return translated position.
	 */
	vector2 pos_in_ancestor(vector2 pos, const widget* ancestor = nullptr);

	//TODO: deprecated, remove.
	Vec2r calcPosInParent(Vec2r pos, const widget* parent = nullptr){
		return this->pos_in_ancestor(pos, parent);
	}

	//TODO: deprecated, remove.
	Vec2r posInAncestor(const widget& ancestor){
		return this->pos_in_ancestor(vector2(0), &ancestor);
	}
};

//TODO: deprecated, remove.
typedef widget Widget;

}


//Widget depends on Container, so need to define it also in all cases when widget is used.
#include "container.hpp"
