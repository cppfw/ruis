/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <string>
#include <set>
#include <memory>

#include <utki/shared.hpp>
#include <utki/types.hpp>

#include <treeml/tree.hpp>

#include <r4/matrix.hpp>
#include <r4/vector.hpp>
#include <r4/rectangle.hpp>

#include "../config.hpp"

#include "../render/texture_2d.hpp"

#include "../util/key.hpp"
#include "../util/events.hpp"
#include "../util/units.hpp"

namespace morda{

class context;
class container;

/**
 * @brief Basic widget class.
 * From GUI script it can be instantiated as "widget".
 * It can have the following parameters:
 * @param layout - layout parameters description. See widget::layout_params for details.
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
class widget : virtual public utki::shared{
	friend class container;
	friend class context;
	friend class gui;
public:
	const std::shared_ptr<morda::context> context;

	/**
	 * @brief Basic layout parameters.
	 */
	class layout_params{
	public:
		/**
		 * @brief Requests minimal dimensions of the widget.
		 * The widget will always be given minimal space it needs to properly draw.
		 */
		constexpr static const real min = real(-1);

		/**
		 * @brief Requests minimal or bigger dimensions of widget.
		 * The widget will be given at least minimal space it needs to properly draw.
		 */
		constexpr static const real max = real(-2);

		/**
		 * @brief Requests widget to be same size as its parent.
		 * Minimal size of the widget is assumed to be 0.
		 */
		constexpr static const real fill = real(-3);

		/**
		 * @param dims - desired dimensions within the layout. Each component can be a non-negative value in pixels or [min, max, fill].
		 */
		layout_params(vector2 dims = vector2(layout_params::min)) :
				dims(dims)
		{}

		/**
		 * @brief desired dimensions.
		 * Components should hold non-negative value in pixels or [min, max, fill].
		 */
		vector2 dims = vector2(layout_params::min);

	public:
		layout_params(const treeml::forest& desc, const morda::units& units);

		virtual ~layout_params()noexcept{}
	};

private:
	container* parent_container = nullptr;

	std::set<unsigned> hovered;

	bool visible = true;

	bool enabled = true;

	morda::rectangle rectangle = {0, 0};

	// clip widgets contents by widget's border if set to true
	bool clip_enabled = false;
public:
	/**
	 * @brief Check if scissor test is enabled for this widget.
	 * @return true if scissor test is enabled.
	 * @return false otherwise.
	 */
	bool is_clip_enabled()const noexcept{
		return this->clip_enabled;
	}

	/**
	 * @brief Enable/Disable scissor test.
	 * @param enable - whether to enable (true) or disable (false) the scissor test.
	 */
	void set_clip_enabled(bool enable)noexcept{
		this->clip_enabled = enable;
	}

private:
	bool cache = false;
	mutable bool cacheDirty = true;
	mutable std::shared_ptr<texture_2d> cache_texture;

	void renderFromCache(const r4::matrix4<float>& matrix)const;

protected:
	void clear_cache();

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

	/**
	 * @brief Render this widget to texture.
	 * @param reuse - try to re-use the existing texture to avoid new texture allocation.
	 *                If dimensions of the passed in texture are different from dimensions
	 *                of the widget then new texture will be allocated.
	 * @return Texture containing rendered widget.
	 */
	std::shared_ptr<texture_2d> render_to_texture(std::shared_ptr<texture_2d> reuse = nullptr)const;

private:
	bool relayoutNeeded = true;

	treeml::forest layout_desc;

	mutable std::unique_ptr<layout_params> layoutParams;
public:
	std::string id;

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 */
	layout_params& get_layout_params();

	/**
	 * @brief Get layout parameters of the widget.
	 * When calling this method the widget should be added to some container or exception will be thrown otherwise.
	 * @return Layout parameters of the widget.
	 */
	const layout_params& get_layout_params_const()const;

	/**
	 * @brief Request re-layout.
	 * Set a flag on the widget indicating to the framework that the widget needs a re-layout.
	 * The layout will be performed before drawing.
	 */
	void invalidate_layout()noexcept;

	/**
	 * @brief Perform layout of the widget.
	 * Override this method to arrange widgets contents if needed.
	 */
	virtual void lay_out(){}

	/**
	 * @brief Check if this widget's contents need to be layed out.
	 * @return true if this widget needs re-layout.
	 * @return false otherwise.
	 */
	bool is_layout_invalid()const noexcept{
		return this->relayoutNeeded;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	const container* parent()const noexcept{
		return this->parent_container;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	container* parent()noexcept{
		return this->parent_container;
	}

	/**
	 * @brief Remove widget from parent container.
	 * Be careful when calling this method from within another method of the widget
	 * as it may result in immediate destruction of the widget if there are no
	 * pointers to this widget left.
	 * @return pointer to this widget.
	 */
	std::shared_ptr<widget> remove_from_parent();

	/**
	 * @brief Replace this widget by another widget.
	 * Replaces this widget in its parent by the given widget.
	 * @param w - widget to replace this one by.
	 * @return Shared pointer to this widget.
	 */
	std::shared_ptr<widget> replace_by(std::shared_ptr<widget> w);

	/**
	 * @brief Check if the widget is hovered by at least one pointer.
	 * @return true if hovered by at least one pointer.
	 * @return false otherwise.
	 */
	bool is_hovered()const noexcept{
		return this->hovered.size() != 0;
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

private:
	void set_hovered(bool is_hovered, unsigned pointer_id);

	void set_unhovered();
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
	 * @brief Get widget's position in absolute coordinates.
	 * @return widget's position in absolute coordinates.
	 */
	morda::vector2 get_absolute_pos()const noexcept;

	/**
	 * @brief Get widget's rectangle in absolute coordinates.
	 * @return widget's absolute coordinates rectangle.
	 */
	morda::rectangle get_absolute_rect()const noexcept;

	/**
	 * @brief Get rectangle occupied by the widget in viewport coordinates.
	 * @param matrix - transformation matrix which transforms point (0,0) to left top corner point of the widget.
	 * @return Rectangle of the widget in viewport coordinates.
	 */
	r4::rectangle<int> compute_viewport_rect(const matrix4& matrix)const noexcept;

	/**
	 * @brief Move widget to position within its parent.
	 * @param new_pos - new widget's position.
	 */
	void move_to(const vector2& new_pos);

	/**
	 * @brief Shift widget within its parent.
	 * @param delta - vector to shift the widget by.
	 */
	void move_by(const vector2& delta){
		this->move_to(this->rectangle.p + delta);
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

	/**
	 * @brief Try get widget by id.
	 * Get widget by ID from the widget hierarchy. Since simple widget cannot have children,
	 * this implementation just checks if id of this widget is the looked up id and if so it returns
	 * pointer to this widget or nullptr otherwise.
	 * @param id - id of the widget to search for.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return pointer to the widget if found.
	 * @return nullptr if there is no widget with given id found.
	 */
	virtual std::shared_ptr<widget> try_get_widget(const std::string& id, bool allow_itself = true)noexcept;

	/**
	 * @brief Try get widget by id.
	 * Same as try_get_widget() but also tries to cast the widget object to a specified class.
	 * @param id - id of the widget to search for.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return pointer to the widget if found.
	 * @return nullptr if there is no widget with given id found or if the widget could not be cast to specified class.
	 */
	template <typename T>
	std::shared_ptr<T> try_get_widget_as(const std::string& id, bool allow_itself = true)noexcept{
		return std::dynamic_pointer_cast<T>(this->try_get_widget(id, allow_itself));
	}

	/**
	 * @brief Try get widget by class.
	 * This function first checks if this widget can be dynamic_cast'ed to the requested class,
	 * and if it can then it returns pointer to this widget. Then it checks if this widget is a container
	 * and searches for requested widget class among the container's child widgets.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return pointer to the first found widget which can be cast to the requested class.
	 */
	template <typename T>
	std::shared_ptr<T> try_get_widget(bool allow_itself = true)noexcept;

	/**
	 * @brief Get widget.
	 * @param id - id of the widget to get.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return reference to the widget.
	 * @throw std::logic_error - if no widget with given id has been found.
	 */
	widget& get_widget(const std::string& id, bool allow_itself = true);

	const widget& get_widget(const std::string& id, bool allow_itself = true)const{
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->get_widget(id, allow_itself);
	}

	/**
	 * @brief Get widget of specific type by its id.
	 * @param id - id of the widget to get.
	 * @param allow_itself - whether it is allowed to return itself in case id and type matches.
	 * @return reference to the requested widget.
	 * @throw std::logic_error - if no widget with given id has been found.
	 * @throw std::bad_cast - if requested widget is not of the specified type.
	 */
	template <typename T>
	T& get_widget_as(const std::string& id, bool allow_itself = true){
		return dynamic_cast<T&>(this->get_widget(id, allow_itself));
	}

	/**
	 * @brief Get widget of specific type.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return reference to the requested widget.
	 */
	template <typename T>
	T& get_widget(bool allow_itself = true);

	/**
	 * @brief Recursively find all widgets of given type.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return list of widgets found.
	 */
	template <class T>
	std::vector<std::shared_ptr<T>> get_all_widgets(bool allow_itself = true);

	/**
	 * @brief Get root widget.
	 * @return reference to the root widget.
	 */
	widget& get_root_widget();

	/**
	 * @brief Get const root widget.
	 * @return reference to the const root widget.
	 */
	const widget& get_root_widget()const{
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->get_root_widget();
	}

public:
	/**
	 * @brief Constructor.
	 * @param c - context to which this widget belongs.
	 * @param desc - widget description.
	 */
	widget(std::shared_ptr<morda::context> c, const treeml::forest& desc);
public:

	virtual ~widget()noexcept{}

	/**
	 * @brief Render widget to screen.
	 * Override this method to render sub-classed widgets. This
	 * implementation does nothing.
	 * @param matrix - transformation matrix to use when rendering.
	 */
	virtual void render(const morda::matrix4& matrix)const{}

private:
	void renderInternal(const morda::matrix4& matrix)const;

private:
	void onKeyInternal(bool isDown, key keyCode);

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
	virtual bool on_key(bool is_down, morda::key key_code){
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

	/**
	 * @brief Called when keyboard input focus changes.
	 */
	virtual void on_focus_change(){}

	/**
	 * @brief Handle mouse button event.
	 * This function is called by framework when widget receives mouse button event.
	 * @param event - mouse button event information, like button, pointer position, etc.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_button(const mouse_button_event& event){
		return false;
	}

	/**
	 * @brief Handle mouse move event.
	 * Called by framework when mouse pointer was moved within the widget.
	 * @param event - mouse move event information, like new pointer position, pointer id.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_move(const mouse_move_event& event){
		return false;
	}

	/**
	 * @brief Handle mouse hover changed event.
	 * Called by framework when mouse pointer enters or leaves the widget boundaries.
	 * @param pointer_id - id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 */
	virtual void on_hover_change(unsigned pointer_id){
//		TRACE(<< "widget::on_hover_change(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	/**
	 * @brief Invoked when widget's size changes.
	 * Default implementation performs laying out of the widget by calling its layout() method.
	 */
	virtual void on_resize(){
//		TRACE(<< "widget::OnResize(): invoked" << std::endl)
		this->lay_out();
	}

	/**
	 * @brief Called when parent of this widget changes.
	 * Called when parent of the widget changes. This happens when widget is
	 * added to or removed from a container.
	 */
	virtual void on_parent_change(){}

	/**
	 * @brief Measure how big a widget wants to be.
	 * Given the space quotum determine what dimensions widget wants to have to properly draw.
	 * @param quotum - space available to widget. If value is negative then a minimum size needed for proper widget drawing is assumed.
	 * @return Measured desired widget dimensions.
	 */
	virtual vector2 measure(const vector2& quotum)const;

public:
	/**
	 * @brief Show/hide widget.
	 * @param visible - whether to show (true) or hide (false) the widget.
	 */
	void set_visible(bool visible);

	/**
	 * @brief Check if widget is visible.
	 * @return true if the widget is visible.
	 * @return false otherwise.
	 */
	bool is_visible()const noexcept{
		return this->visible;
	}

	/**
	 * @brief Enable/disable the widget.
	 * @param enable - whether to enable (true) or disable (false) the widget.
	 */
	void set_enabled(bool enable);

	/**
	 * @brief Check if the widget is currently enabled.
	 * @return true if the widget is currently enabled.
	 * @return false otherwise.
	 */
	bool is_enabled()const noexcept{
		return this->enabled;
	}

	/**
	 * @brief Invoked when enabled state of the widget changes.
	 */
	virtual void on_enable_change(){}

	/**
	 * @brief Check if widget can receive user input.
	 * Checks if widget is enabled and visible, so it can receive user input.
	 * @return true if widget can receive user input.
	 * @return false otherwise.
	 */
	bool is_interactive()const noexcept{
		return this->is_enabled() && this->is_visible();
	}

	/**
	 * @brief Check if point is within the widget bounds.
	 * @param pos - point to check in widget coordinates.
	 * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
	 */
	bool overlaps(const morda::vector2& pos)const noexcept{
		return morda::rectangle(morda::vector2(0, 0), this->rect().d).overlaps(pos);
	}

	/**
	 * @brief Find ancestor with given id and of given class.
	 * @param id - id to look for. If empty, then any ancestor of the given class will match.
	 * @return pointer to the found ancestor widget.
	 * @return nullptr if no ancestor which satisfies the conditions was found.
	 */
	template <class T> T* try_get_ancestor(const std::string& id = std::string()); // defined in container.hpp

	/**
	 * @brief Get ancestor widget with given id.
	 * @param id - id to look for.
	 * @return pointer to the found ancestor widget.
	 * @return nullptr in case no ancestor widget with given id was found.
	 */
	widget* try_get_ancestor(const std::string& id);

	const widget* try_get_ancestor(const std::string& id)const{
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->try_get_ancestor(id);
	}

	/**
	 * @brief Get ancestor widget with given id.
	 * @param id - id to look for.
	 * @return reference to the found ancestor widget.
	 * @throw std::logic_error - in case no ancestor widget with given id was found.
	 */
	widget& get_ancestor(const std::string& id);

	const widget& get_ancestor(const std::string& id)const{
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->get_ancestor(id);
	}

	/**
	 * @brief Calculate position in ancestor coordinates.
	 * @param pos - position to translate to ancestor coordinates.
	 * @param ancestor - ancestor of the widget hierarchy relatively to which the translation is to be done.
	 *        If null then it will go down till the root widget.
	 * @return translated position.
	 */
	vector2 pos_in_ancestor(vector2 pos, const widget* ancestor = nullptr);
};

}

// widget depends on container, so need to define it also in all cases when widget is used.
#include "container.hpp"
