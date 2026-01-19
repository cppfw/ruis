/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <memory>
#include <set>
#include <string>

#include <r4/matrix.hpp>
#include <r4/rectangle.hpp>
#include <r4/vector.hpp>
#include <tml/tree.hpp>
#include <utki/shared.hpp>
#include <utki/types.hpp>

#include "../config.hpp"
#include "../layout/parameters.hpp"
#include "../render/texture_2d.hpp"
#include "../util/events.hpp"
#include "../util/key.hpp"
#include "../util/units.hpp"

namespace ruis {

class context;
class container;

namespace layout {
class layout;
} // namespace layout

/**
 * @brief Base widget class.
 */
class widget : virtual public utki::shared
{
	friend class container;
	friend class context;
	friend class gui;
	friend class layout::layout;

public:
	const utki::shared_ref<ruis::context> context;

	/**
	 * @brief Shorthand alias for this->context.
	 * @return Reference to the context of this widget.
	 */
	ruis::context& ctx() noexcept
	{
		return this->context;
	}

	/**
	 * @brief Shorthand alias for this->context.
	 * @return Constant reference to the context of this widget.
	 */
	const ruis::context& ctx() const noexcept
	{
		return this->context;
	}

private:
	// TODO: make weak_ptr?
	// NOLINTNEXTLINE(clang-analyzer-webkit.NoUncountedMemberChecker, "false-positive")
	container* parent_container = nullptr;

	std::set<unsigned> hovered;

public:
	/**
	 * @brief Check if scissor test is enabled for this widget.
	 * @return true if scissor test is enabled.
	 * @return false otherwise.
	 */
	bool is_clip_enabled() const noexcept
	{
		return this->params.clip;
	}

	/**
	 * @brief Enable/Disable scissor test.
	 * @param enable - whether to enable (true) or disable (false) the scissor test.
	 */
	void set_clip(bool enable) noexcept
	{
		this->params.clip = enable;
	}

private:
	mutable bool cache_dirty = true;
	mutable std::shared_ptr<render::frame_buffer> cache_frame_buffer;

	void render_from_cache(const mat4& matrix) const;

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
	void set_cache(bool enabled) noexcept
	{
		this->params.cache = enabled;
	}

	/**
	 * @brief Render this widget to texture.
	 * @param reuse - try to re-use the existing texture to avoid new texture allocation.
	 *                If dimensions of the passed in texture are different from dimensions
	 *                of the widget then new texture will be allocated.
	 * @return Texture containing rendered widget.
	 */
	utki::shared_ref<render::frame_buffer> render_to_texture( //
		std::shared_ptr<render::frame_buffer> reuse = nullptr
	) const;

private:
	bool layout_dirty = true;

public:
	std::string_view id() const
	{
		return this->params.id;
	}

	/**
	 * @brief Get layout parameters of the widget.
	 * This method invalidates layout.
	 * @return Layout parameters of the widget.
	 */
	layout::parameters& get_layout_params();

	/**
	 * @brief Get constant layout parameters of the widget.
	 * @return Constant layout parameters of the widget.
	 */
	const layout::parameters& get_layout_params_const() const
	{
		return this->layout_params;
	}

	/**
	 * @brief Request re-layout.
	 * Set a flag on the widget indicating to the framework that the widget needs a re-layout.
	 * The layout will be performed before drawing.
	 * If the widget is added to some parent, the flag will be set on all ancestors down to the root as well.
	 */
	void invalidate_layout() noexcept;

	/**
	 * @brief Called when layouting of the widget is needed.
	 * Override this method to arrange widget's contents if needed.
	 */
	virtual void on_lay_out() {}

	/**
	 * @brief Perform layouting of the widget's contents.
	 */
	void lay_out();

	/**
	 * @brief Check if this widget's contents need to be layed out.
	 * @return true if this widget needs re-layouting.
	 * @return false otherwise.
	 */
	bool is_layout_dirty() const noexcept
	{
		return this->layout_dirty;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	const container* parent() const noexcept
	{
		return this->parent_container;
	}

	/**
	 * @brief Get widget's parent container.
	 * @return Widget's parent container.
	 */
	container* parent() noexcept
	{
		return this->parent_container;
	}

	/**
	 * @brief Remove widget from parent container.
	 * Be careful when calling this method from within another method of the widget
	 * as it may result in immediate destruction of the widget if there are no
	 * references to this widget left.
	 * @return pointer to this widget.
	 */
	utki::shared_ref<widget> remove_from_parent();

	/**
	 * @brief Replace this widget by another widget.
	 * Replaces this widget in its parent by the given widget.
	 * Be careful when calling this method from within another method of the widget
	 * as it may result in immediate destruction of the widget if there are no
	 * references to this widget left.
	 * @param w - widget to replace this one by.
	 * @return Shared reference to this widget.
	 */
	utki::shared_ref<widget> replace_by(const utki::shared_ref<widget>& w);

	/**
	 * @brief Check if the widget is hovered by at least one pointer.
	 * @return true if hovered by at least one pointer.
	 * @return false otherwise.
	 */
	bool is_hovered() const noexcept
	{
		return this->hovered.size() != 0;
	}

	/**
	 * @brief Check if widget is hovered by given pointer.
	 * @param pointer_id - pointer id to check against.
	 * @return true if widget is hovered by given pointer id.
	 * @return false otherwise.
	 */
	bool is_hovered(unsigned pointer_id) const noexcept
	{
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
	const ruis::rect& rect() const noexcept
	{
		return this->params.rectangle;
	}

	/**
	 * @brief Get widget's position in absolute coordinates.
	 * @return widget's position in absolute coordinates.
	 */
	ruis::vec2 get_absolute_pos() const noexcept;

	/**
	 * @brief Get widget's rectangle in absolute coordinates.
	 * @return widget's absolute coordinates rectangle.
	 */
	ruis::rect get_absolute_rect() const noexcept;

	/**
	 * @brief Get rectangle occupied by the widget in viewport coordinates.
	 * @param matrix - transformation matrix which transforms point (0,0) to left top corner point of the widget.
	 * @return Rectangle of the widget in viewport coordinates.
	 */
	r4::rectangle<uint32_t> compute_viewport_rect(const mat4& matrix) const noexcept;

	/**
	 * @brief Move widget to position within its parent.
	 * @param new_pos - new widget's position.
	 */
	void move_to(const vec2& new_pos);

	/**
	 * @brief Shift widget within its parent.
	 * @param delta - vector to shift the widget by.
	 */
	void move_by(const vec2& delta)
	{
		this->move_to(this->params.rectangle.p + delta);
	}

	/**
	 * @brief Set new dimensions of the widget.
	 * @param new_dims - new dimensions of the widget.
	 */
	void resize(const vec2& new_dims);

	/**
	 * @brief Extend or shrink the widget dimensions by given value.
	 * @param delta - value to change the widget dimensions by.
	 */
	void resize_by(const vec2& delta)
	{
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
	virtual std::shared_ptr<widget> try_get_widget(std::string_view id, bool allow_itself = true) noexcept;

	/**
	 * @brief Try get widget by id.
	 * Same as try_get_widget() but also tries to cast the widget object to a specified class.
	 * @param id - id of the widget to search for.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return pointer to the widget if found.
	 * @return nullptr if there is no widget with given id found or if the widget could not be cast to specified class.
	 */
	template <typename widget_type>
	std::shared_ptr<widget_type> try_get_widget_as(std::string_view id, bool allow_itself = true) noexcept
	{
		return std::dynamic_pointer_cast<widget_type>(this->try_get_widget(id, allow_itself));
	}

	/**
	 * @brief Try get widget by class.
	 * This function first checks if this widget can be dynamic_cast'ed to the requested class,
	 * and if it can then it returns pointer to this widget. Then it checks if this widget is a container
	 * and searches for requested widget class among the container's child widgets.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return pointer to the first found widget which can be cast to the requested class.
	 */
	template <typename widget_type>
	std::shared_ptr<widget_type> try_get_widget(bool allow_itself = true) noexcept;

	/**
	 * @brief Get widget.
	 * @param id - id of the widget to get.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return reference to the widget.
	 * @throw std::logic_error - if no widget with given id has been found.
	 */
	widget& get_widget(std::string_view id, bool allow_itself = true);

	const widget& get_widget(std::string_view id, bool allow_itself = true) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
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
	template <typename widget_type>
	widget_type& get_widget_as(std::string_view id, bool allow_itself = true)
	{
		return dynamic_cast<widget_type&>(this->get_widget(id, allow_itself));
	}

	/**
	 * @brief Get widget of specific type.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return reference to the requested widget.
	 */
	template <typename widget_type>
	widget_type& get_widget(bool allow_itself = true);

	/**
	 * @brief Recursively find all widgets of given type.
	 * @param allow_itself - whether it is allowed to return itself in case type matches.
	 * @return list of widgets found.
	 */
	template <class widget_type>
	std::vector<utki::shared_ref<widget_type>> get_all_widgets(bool allow_itself = true);

	/**
	 * @brief Get root widget.
	 * @return reference to the root widget.
	 */
	widget& get_root_widget();

	/**
	 * @brief Get const root widget.
	 * @return reference to the const root widget.
	 */
	const widget& get_root_widget() const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->get_root_widget();
	}

public:
	struct parameters {
		std::string id;

		/**
		 * @brief Initial widget rectangle within its parent.
		 * Default position is (0, 0) and default size is (-1, -1).
		 * Negative size values assure that if the widget is governed by a layout
		 * it will be resized (and will recieve on_resize() notification) before it's first render.
		 */
		ruis::rect rectangle = {0, -1};

		/**
		 * @brief Clip widgets contents by widget's border.
		 */
		bool clip = false;

		/**
		 * @brief Enable caching widget's image to texture.
		 */
		bool cache = false;

		/**
		 * @brief Visibility of the widget.
		 */
		bool visible = true;

		/**
		 * @brief Widget's enabled state.
		 */
		bool enabled = true;

		/**
		 * @brief Usage of depth buffer for rendering the widget.
		 */
		bool depth = false;
	};

	struct all_parameters {
		layout::parameters layout_params;
		parameters widget_params;
	};

private:
	layout::parameters layout_params;
	parameters params;

public:
	/**
	 * @brief Get currently set parameters.
	 * @return Constant reference to the widget's parameters.
	 */
	const parameters& get_params() const noexcept
	{
		return this->params;
	}

protected:
	widget(
		utki::shared_ref<ruis::context> context, //
		layout::parameters layout_params,
		parameters params
	);

public:
	widget(const widget&) = delete;
	widget& operator=(const widget&) = delete;

	widget(widget&&) = delete;
	widget& operator=(widget&&) = delete;

public:
	~widget() override = default;

	/**
	 * @brief Render widget to screen.
	 * Override this method to render sub-classed widgets. This
	 * implementation does nothing.
	 * @param matrix - transformation matrix to use when rendering.
	 */
	virtual void render(const ruis::mat4& matrix) const {}

private:
	void render_internal(const ruis::mat4& matrix) const;

private:
	void on_key_internal(const ruis::key_event& e);

private:
	bool focused = false;

public:
	/**
	 * @brief Handle keyboard key event.
	 * This method is called by framework when a widget is requested to handle a key event.
	 * So, the widget is either a focused widget or root widget.
	 * @param e - key event information structure.
	 * @return true to consume event and prevent its further propagation.
	 * @return false to allow the event to be propagated further.
	 */
	virtual bool on_key(const ruis::key_event& e)
	{
		return false;
	}

	/**
	 * @brief Set this widget as focused.
	 * Focused widget will receive keyboard events.
	 */
	void focus() noexcept;

	/**
	 * @brief Unfocus this widget.
	 */
	void unfocus() noexcept;

	/**
	 * @brief Check if this widget is focused.
	 * @return true if widget is focused.
	 * @return false otherwise.
	 */
	bool is_focused() const noexcept
	{
		return this->focused;
	}

	/**
	 * @brief Called when keyboard input focus changes.
	 */
	virtual void on_focus_change() {}

	/**
	 * @brief Handle mouse button event.
	 * This function is called by framework when widget receives mouse button event.
	 * @param event - mouse button event information, like button, pointer position, etc.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_button(const mouse_button_event& event)
	{
		return false;
	}

	/**
	 * @brief Handle mouse move event.
	 * Called by framework when mouse pointer was moved within the widget.
	 * @param event - mouse move event information, like new pointer position, pointer id.
	 * @return true to consume the event and prevent its further propagation.
	 * @return false to allow the event to be propagated to underlying widgets.
	 */
	virtual bool on_mouse_move(const mouse_move_event& event)
	{
		return false;
	}

	/**
	 * @brief Handle mouse hover changed event.
	 * Called by framework when mouse pointer enters or leaves the widget boundaries.
	 * @param pointer_id - id of the mouse pointer on systems with multiple mouse pointers, like multitouch screens.
	 */
	virtual void on_hovered_change(unsigned pointer_id)
	{
		//		TRACE(<< "widget::on_hover_change(): this->IsHovered() = " << this->IsHovered() << std::endl)
	}

	/**
	 * @brief Invoked when widget's size changes.
	 * Default implementation performs laying out of the widget by calling its lay_out() method.
	 */
	virtual void on_resize();

	/**
	 * @brief Called when parent of this widget changes.
	 * Called when parent of the widget changes. This happens when widget is
	 * added to or removed from a container.
	 */
	virtual void on_parent_change() {}

	/**
	 * @brief Measure how big a widget wants to be.
	 * Given the space quotum determine what dimensions widget wants to have to properly draw.
	 * @param quotum - space available to widget. If value is negative then a minimum size needed for proper widget
	 * drawing is assumed.
	 * @return Measured desired widget dimensions.
	 */
	virtual vec2 measure(const vec2& quotum) const;

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
	bool is_visible() const noexcept
	{
		return this->params.visible;
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
	bool is_enabled() const noexcept
	{
		return this->params.enabled;
	}

	/**
	 * @brief Invoked when enabled state of the widget changes.
	 */
	virtual void on_enabled_change() {}

	/**
	 * @brief Check if widget can receive user input.
	 * Checks if widget is enabled and visible, so it can receive user input.
	 * @return true if widget can receive user input.
	 * @return false otherwise.
	 */
	bool is_interactive() const noexcept
	{
		return this->is_enabled() && this->is_visible();
	}

	/**
	 * @brief Check if point is within the widget bounds.
	 * @param pos - point to check in widget coordinates.
	 * @return true if point is inside of the widget boundaries.
	 * @return false otherwise.
	 */
	bool overlaps(const ruis::vec2& pos) const noexcept
	{
		return ruis::rect(ruis::vec2(0, 0), this->rect().d).overlaps(pos);
	}

	/**
	 * @brief Find ancestor with given id and of given class.
	 * @param id - id to look for. If empty, then any ancestor of the given class will match.
	 * @return pointer to the found ancestor widget.
	 * @return nullptr if no ancestor which satisfies the conditions was found.
	 */
	template <typename widget_type>
	widget_type* try_get_ancestor(const std::string& id = std::string()); // defined in container.hpp

	/**
	 * @brief Get ancestor widget with given id.
	 * @param id - id to look for.
	 * @return pointer to the found ancestor widget.
	 * @return nullptr in case no ancestor widget with given id was found.
	 */
	widget* try_get_ancestor(const std::string& id);

	const widget* try_get_ancestor(const std::string& id) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->try_get_ancestor(id);
	}

	/**
	 * @brief Get ancestor widget with given id.
	 * @param id - id to look for.
	 * @return reference to the found ancestor widget.
	 * @throw std::logic_error - in case no ancestor widget with given id was found.
	 */
	widget& get_ancestor(const std::string& id);

	const widget& get_ancestor(const std::string& id) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<utki::remove_const_pointer<decltype(this)>::type*>(this)->get_ancestor(id);
	}

	/**
	 * @brief Calculate position in ancestor coordinates.
	 * @param pos - position to translate to ancestor coordinates.
	 * @param ancestor - ancestor of the widget hierarchy relatively to which the translation is to be done.
	 *        If null then it will go down till the root widget.
	 * @return translated position.
	 */
	vec2 pos_in_ancestor(vec2 pos, const widget* ancestor = nullptr);

public:
	/**
	 * @brief Reload widget's resources.
	 * Requests widget to reload it's resources due to poissible change
	 * in mounted resource packs of the resource_loader, or change in context::units values.
	 * This function will invalidate the layout and call virtual on_reload().
	 */
	void reload();

	/**
	 * @brief Called when the widget is requested to reload it's resources.
	 * This function is called when widget is requesterd to reload it's resources.
	 * This can be useful when, for example, there was a change to mounted resource
	 * packs of the resource_loader, or change of context::units values.
	 * The widget is expected to reload it's resources loaded via resource_loader.
	 */
	virtual void on_reload() {}

	template <typename resource_type>
	void reload(std::shared_ptr<resource_type>& p);

	template <typename resource_type>
	void reload(utki::shared_ref<resource_type>& p);
};

/**
 * @brief Calculate basic dimensions of widget.
 * Calculates basic dimensions of given widget if it would be placed to
 * a container with given dimensions and given layout parameters, basically this is just
 * resolving of 'min', 'max' and 'fill' special values of dimensions.
 * @param w - widget to calculate dimensions for.
 * @param parent_dims - parent widget dimensions.
 * @return Dimensions of widget.
 */
vec2 dims_for_widget(const widget& w, const vec2& parent_dims);

} // namespace ruis

// include definitions for forward declared classes
#include "../context.hpp"

#include "container.hpp"

template <typename resource_type>
void ruis::widget::reload(std::shared_ptr<resource_type>& p)
{
	if (!p) {
		return;
	}
	p = this->context.get().loader().load<resource_type>(p->get_id());
}

template <typename resource_type>
void ruis::widget::reload(utki::shared_ref<resource_type>& p)
{
	p = this->context.get().loader().load<resource_type>(p->get_id());
}
