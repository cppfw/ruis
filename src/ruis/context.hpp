/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "render/renderer.hpp"
#include "util/events.hpp"
#include "util/mouse_cursor.hpp"
#include "util/mouse_cursor_manager.hpp"
#include "util/units.hpp"

#include "inflater.hpp"
#include "layout_factory.hpp"
#include "resource_loader.hpp"
#include "updateable.hpp"

namespace ruis {

class context : public std::enable_shared_from_this<context>
{
	friend class widget;
	friend class gui;

	std::weak_ptr<widget> focused_widget;

	void set_focused_widget(const std::shared_ptr<widget>& w);

public:
	const utki::shared_ref<ruis::renderer> renderer;

	const utki::shared_ref<ruis::updater> updater;

	const std::function<void(std::function<void()>)> run_from_ui_thread;

	mouse_cursor_manager cursor_manager;

	/**
	 * @brief Instantiation of the resource loader.
	 */
	resource_loader loader;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	ruis::inflater inflater;

	/**
	 * @brief Instance of layout factory.
	 */
	ruis::layout_factory layout_factory;

	/**
	 * @brief Constructor.
	 * @param r - renderer implementation.
	 * @param u - updater to use along with this context.
	 * @param run_from_ui_thread_function - function to use when posting an action to UI thread is needed.
	 * @param set_mouse_cursor_function - function to use for setting the mouse cursor.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_pp - desired dots per perception pixel.
	 */
	context(
		const utki::shared_ref<ruis::renderer>& r,
		const utki::shared_ref<ruis::updater>& u,
		std::function<void(std::function<void()>)> run_from_ui_thread_function,
		std::function<void(ruis::mouse_cursor)> set_mouse_cursor_function,
		real dots_per_inch,
		real dots_per_pp
	);

	context(const context&) = delete;
	context& operator=(const context&) = delete;

	context(context&&) = delete;
	context& operator=(context&&) = delete;

	~context() = default;

	const ruis::units units;
};

} // namespace ruis

// TODO:?
// #include "widget.hpp"
