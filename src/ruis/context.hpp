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
#include "util/mouse_cursor_stack.hpp"
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

	const std::function<void(std::function<void()>)> post_to_ui_thread_function;

public:
	/**
	 * @brief Post procedure to UI thread.
	 * Posts a procedure to UI thread to be run later. The procedure will be run
	 * within this or next cycle of the UI main loop.
	 * @param proc - procedure to post to the UI thread.
	 */
	void post_to_ui_thread(std::function<void()> proc);

	/**
	 * @brief Resource loader.
	 * Allows loading and managing life time of resources.
	 */
	const utki::shared_ref<resource_loader> res_loader;

	/**
	 * @brief Shorthand alias for resource loader.
	 * @return this->res_loader.get().
	 */
	resource_loader& loader() noexcept
	{
		return this->res_loader.get();
	}

	/**
	 * @brief Renderer used for drawing in this GUI context.
	 * This is the same renderer as in the supplied resource loader.
	 */
	const utki::shared_ref<ruis::render::renderer> renderer;

	/**
	 * @brief Shorthand alias for renderer.
	 * @return this->renderer.get().
	 */
	ruis::render::renderer& ren() noexcept
	{
		return this->renderer.get();
	}

	/**
	 * @brief Updater which updates updatables from within UI thread.
	 * The updater calls active updatables periodically from the UI thread.
	 */
	// potentially, updater can be shared between contexts, this is why it is shared_ref
	const utki::shared_ref<ruis::updater> updater;

	/**
	 * @brief Mouse cursor stack.
	 * It allows changing shape of the mouse cursor.
	 */
	mouse_cursor_stack cursor_stack;

	/**
	 * @brief current localization.
	 * Vocabulary of localized strings.
	 */
	utki::shared_ref<ruis::localization> localization;

	/**
	 * @brief Unit values.
	 */
	ruis::units units;

	struct parameters {
		std::function<void(std::function<void()>)> post_to_ui_thread_function;
		std::function<void(ruis::mouse_cursor)> set_mouse_cursor_function;
		ruis::units units;
		utki::shared_ref<ruis::localization> localization = utki::make_shared<ruis::localization>();
	};

	/**
	 * @brief Constructor.
	 * @param res_loader - resource loader to use for this context.
	 * @param updater - updater to use along with this context.
	 * @param params - context parameters.
	 */
	context(
		utki::shared_ref<ruis::resource_loader> res_loader,
		utki::shared_ref<ruis::updater> updater,
		parameters params
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
