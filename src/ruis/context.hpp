/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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
#include "util/localization.hpp"
#include "util/mouse_cursor.hpp"
#include "util/mouse_cursor_manager.hpp"
#include "util/units.hpp"

#include "resource_loader.hpp"
#include "updateable.hpp"

namespace ruis {

class widget;

class context : public std::enable_shared_from_this<context>
{
	friend class widget;
	friend class gui;

	std::weak_ptr<widget> focused_widget;

	void set_focused_widget(const std::shared_ptr<widget>& w);

public:
	const utki::shared_ref<ruis::render::renderer> renderer;

	// potentially, updater can be shared between contexts, this is why it is shared_ref
	const utki::shared_ref<ruis::updater> updater;

	const std::function<void(std::function<void()>)> post_to_ui_thread;

	mouse_cursor_manager cursor_manager;

	/**
	 * @brief Instantiation of the resource loader.
	 */
	// TODO: make shared_ref, dependency inject renderer (for textures loading)
	resource_loader loader;

	/**
	 * @brief current localization.
	 * Vocabulary of localized strings.
	 */
	utki::shared_ref<ruis::localization> localization;

	/**
	 * @brief Unit values.
	 */
	ruis::units units;

	constexpr static const auto default_dots_per_inch = 96;
	constexpr static const auto default_dots_per_pp = 1;

	/**
	 * @brief Constructor.
	 * @param renderer - renderer implementation.
	 * @param updater - updater to use along with this context.
	 * @param post_to_ui_thread_function - function for posting an action to UI thread.
	 * @param set_mouse_cursor_function - function for setting the mouse cursor.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_pp - desired dots per perception pixel.
	 * @param localization - localization instance.
	 */
	context(
		utki::shared_ref<ruis::render::renderer> renderer,
		utki::shared_ref<ruis::updater> updater,
		std::function<void(std::function<void()>)> post_to_ui_thread_function,
		std::function<void(ruis::mouse_cursor)> set_mouse_cursor_function,
		real dots_per_inch = default_dots_per_inch,
		real dots_per_pp = default_dots_per_pp,
		utki::shared_ref<ruis::localization> localization = utki::make_shared<ruis::localization>()
	);

	context(const context&) = delete;
	context& operator=(const context&) = delete;

	context(context&&) = delete;
	context& operator=(context&&) = delete;

	~context() = default;
};

} // namespace ruis

// TODO:?
// #include "widget.hpp"
