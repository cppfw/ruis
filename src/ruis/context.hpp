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
#include "style/style_provider.hpp"
#include "util/events.hpp"
#include "util/localization.hpp"
#include "util/mouse_cursor.hpp"
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
	 * @brief Default style to be used for this context.
	 */
	const utki::shared_ref<ruis::style_provider> style_provider;

	/**
	 * @brief Shorthand alias for style_provider.
	 * @return this->style_provider.get().
	 */
	const ruis::style_provider& style() const noexcept
	{
		return this->style_provider.get();
	}

	/**
	 * @brief Shorthand alias for resource loader.
	 * @return this->res_loader.get().
	 */
	resource_loader& loader() noexcept
	{
		return this->style().res_loader.get();
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
	 * @brief Shorthand alias for renderer.
	 * @return this->renderer.get().
	 */
	ruis::render::renderer& ren() const noexcept
	{
		return this->renderer.get();
	}

	/**
	 * @brief Shorthand alias for native_window.
	 * @return The native window of this GUI context.
	 */
	ruis::render::native_window& window() noexcept
	{
		return this->ren().ctx().native_window;
	}

	/**
	 * @brief Updater which updates updatables from within UI thread.
	 * The updater calls active updatables periodically from the UI thread.
	 */
	// potentially, updater can be shared between contexts, this is why it is shared_ref
	const utki::shared_ref<ruis::updater> updater;

	/**
	 * @brief current localization.
	 * Vocabulary of localized strings.
	 */
	utki::shared_ref<ruis::localization> localization;

	/**
	 * @brief Shorthand alias for localization.
	 * @return this->localization.get().
	 */
	const ruis::localization& loc() const noexcept
	{
		return this->localization.get();
	}

	/**
	 * @brief Unit values.
	 */
	ruis::units units;

	struct parameters {
		std::function<void(std::function<void()>)> post_to_ui_thread_function;
		ruis::units units;
		utki::shared_ref<ruis::localization> localization = utki::make_shared<ruis::localization>();
		// TODO: add style_provider field
	};

	/**
	 * @brief Constructor.
	 * @param style_provider - style provider to use for this context.
	 * @param renderer - ruis  renderer for this context.
	 * @param updater - updater to use along with this context.
	 * @param params - context parameters.
	 */
	context(
		utki::shared_ref<ruis::style_provider> style_provider,
		utki::shared_ref<ruis::render::renderer> renderer,
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
