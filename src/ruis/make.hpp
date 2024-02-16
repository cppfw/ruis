/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <utki/config.hpp>

#include "widgets/group/book.hpp"
#include "widgets/group/list.hpp"
#include "widgets/group/overlay.hpp"
#include "widgets/group/scroll_area.hpp"
#include "widgets/label/gradient.hpp"
#include "widgets/label/image.hpp"
#include "widgets/label/nine_patch.hpp"
#include "widgets/label/text.hpp"
#include "widgets/proxy/click_proxy.hpp"
#include "widgets/proxy/key_proxy.hpp"
#include "widgets/proxy/min_proxy.hpp"
#include "widgets/proxy/mouse_proxy.hpp"
#include "widgets/proxy/resize_proxy.hpp"

#include "container.hpp"
#include "lp.hpp"
#include "widget.hpp"

namespace ruis::make {

// core widgets

inline utki::shared_ref<ruis::widget> widget( //
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters params
)
{
	return utki::make_shared<ruis::widget>(std::move(context), std::move(params));
}

inline utki::shared_ref<ruis::widget> container(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::container::parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::container>(
		std::move(context),
		std::move(widget_params),
		std::move(params),
		children
	);
}

// proxy widgets

inline utki::shared_ref<ruis::widget> mouse_proxy(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters params
)
{
	return utki::make_shared<ruis::mouse_proxy>(std::move(context), std::move(params));
}

inline utki::shared_ref<ruis::widget> click_proxy(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters params
)
{
	return utki::make_shared<ruis::click_proxy>(std::move(context), std::move(params));
}

inline utki::shared_ref<ruis::widget> key_proxy(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::container::parameters container_params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::key_proxy>(
		std::move(context),
		std::move(widget_params),
		std::move(container_params),
		children
	);
}

inline utki::shared_ref<ruis::widget> resize_proxy( //
	utki::shared_ref<ruis::context> context,
	widget::parameters params
)
{
	return utki::make_shared<ruis::resize_proxy>(std::move(context), std::move(params));
}

inline utki::shared_ref<ruis::widget> min_proxy(
	utki::shared_ref<ruis::context> context,
	widget::parameters widget_params,
	min_proxy::parameters params
)
{
	return utki::make_shared<ruis::min_proxy>(std::move(context), std::move(widget_params), std::move(params));
}

// group widgets

inline utki::shared_ref<ruis::widget> scroll_area(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::container::parameters container_params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::scroll_area>(
		std::move(context),
		std::move(widget_params),
		std::move(container_params),
		children
	);
}

inline utki::shared_ref<ruis::widget> overlay(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	utki::span<const utki::shared_ref<ruis::widget>> children
)
{
	return utki::make_shared<ruis::overlay>(std::move(context), std::move(widget_params), children);
}

inline utki::shared_ref<ruis::widget> list(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::oriented::parameters oriented_params
)
{
	return utki::make_shared<ruis::list>(std::move(context), std::move(widget_params), std::move(oriented_params));
}

inline utki::shared_ref<ruis::book> book(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params
)
{
	return utki::make_shared<ruis::book>(std::move(context), std::move(widget_params));
}

// label widgets

inline utki::shared_ref<ruis::widget> text(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	std::u32string text = {},
	ruis::color_widget::parameters color_widget_params = {},
	ruis::text_widget::parameters text_widget_params = {}
)
{
	return utki::make_shared<ruis::text>(
		std::move(context),
		std::move(widget_params),
		std::move(text_widget_params),
		std::move(color_widget_params),
		std::move(text)
	);
}

inline utki::shared_ref<ruis::widget> gradient(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::gradient::parameters params
)
{
	return utki::make_shared<ruis::gradient>(std::move(context), std::move(widget_params), std::move(params));
}

inline utki::shared_ref<ruis::widget> image(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::image::parameters params = {},
	ruis::blending_widget::parameters blending_widget_params = {}
)
{
	return utki::make_shared<ruis::image>(
		std::move(context),
		std::move(widget_params),
		std::move(blending_widget_params),
		std::move(params)
	);
}

inline utki::shared_ref<ruis::widget> nine_patch(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::nine_patch::parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children,
	blending_widget::parameters blending_widget_params = {}
)
{
	return utki::make_shared<ruis::nine_patch>(
		std::move(context),
		std::move(widget_params),
		std::move(blending_widget_params),
		std::move(params),
		children
	);
}

} // namespace ruis::make
