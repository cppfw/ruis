#pragma once

#include "render/renderer.hpp"

#include "util/events.hpp"
#include "util/mouse_cursor.hpp"
#include "util/units.hpp"
#include "util/mouse_cursor_manager.hpp"

#include "updateable.hpp"
#include "inflater.hpp"
#include "resource_loader.hpp"

namespace morda{

class context : public utki::shared{
	friend class widget;
	friend class gui;

	std::weak_ptr<widget> focused_widget;

	void set_focused_widget(std::shared_ptr<widget> w);

public:
	const std::shared_ptr<morda::renderer> renderer;

	const std::shared_ptr<morda::updater> updater;

	const std::function<void(std::function<void()>&&)> run_from_ui_thread;

	mouse_cursor_manager cursor_manager;

	/**
	 * @brief Instantiation of the resource loader.
	 */
	resource_loader loader;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	morda::inflater inflater;

	/**
	 * @brief Constructor.
	 * @param r - renderer implementation.
	 * @param u - updater to use along with this context.
	 * @param run_from_ui_thread_function - function to use when posting an action to UI thread is needed.
	 * @param set_mouse_cursor_function - function to use for setting the mouse cursor.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_dp - desired dots per density pixel.
	 */
	context(
			std::shared_ptr<morda::renderer> r,
			std::shared_ptr<morda::updater> u,
			std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
			std::function<void(morda::mouse_cursor)>&& set_mouse_cursor_function,
			real dots_per_inch,
			real dots_per_dp
		);
	
	context(const context&) = delete;
	context(context&&) = delete;
	context& operator=(const context&) = delete;

	const morda::units units;
};

}
