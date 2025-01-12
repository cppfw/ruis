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

#include <functional>

#include "../../res/nine_patch.hpp"
#include "../../util/oriented.hpp"
#include "../base/fraction_band_widget.hpp"
#include "../widget.hpp"

// disable stupid warnings
#if CFG_COMPILER == CFG_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif

namespace ruis {

class scroll_bar :
	public fraction_band_widget, //
	protected oriented,
	private container
{
	widget& handle;

	bool is_grabbed = false;
	float grab_point;

public:
	struct parameters {
		std::shared_ptr<res::nine_patch> background;
		std::shared_ptr<res::nine_patch> handle;
	};

private:
	parameters params;

	void on_fraction_change() override;

	void on_band_change() override;

public:
	scroll_bar(const utki::shared_ref<ruis::context>& c, const tml::forest& desc, bool vertical);

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		fraction_widget::parameters fraction_params;
		fraction_band_widget::parameters fraction_band_params;
		oriented::parameters oriented_params;
		parameters scroll_bar_params;
	};

	scroll_bar(utki::shared_ref<ruis::context> c, all_parameters params);

public:
	scroll_bar(const scroll_bar&) = delete;
	scroll_bar& operator=(const scroll_bar&) = delete;

	scroll_bar(scroll_bar&&) = delete;
	scroll_bar& operator=(scroll_bar&&) = delete;

	~scroll_bar() override = default;

private:
	void on_lay_out() override;
};

namespace make {
inline utki::shared_ref<scroll_bar> scroll_bar(utki::shared_ref<context> context, scroll_bar::all_parameters params)
{
	return utki::make_shared<ruis::scroll_bar>(std::move(context), std::move(params));
}
} // namespace make

// TODO: deprecated
class vertical_scroll_bar : public scroll_bar
{
public:
	vertical_scroll_bar(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		scroll_bar(this->context, desc, true)
	{}

	vertical_scroll_bar(const vertical_scroll_bar&) = delete;
	vertical_scroll_bar& operator=(const vertical_scroll_bar&) = delete;

	vertical_scroll_bar(vertical_scroll_bar&&) = delete;
	vertical_scroll_bar& operator=(vertical_scroll_bar&&) = delete;

	~vertical_scroll_bar() override = default;
};

// TODO: deprecated
class horizontal_scroll_bar : public scroll_bar
{
public:
	horizontal_scroll_bar(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		scroll_bar(this->context, desc, false)
	{}

	horizontal_scroll_bar(const horizontal_scroll_bar&) = delete;
	horizontal_scroll_bar& operator=(const horizontal_scroll_bar&) = delete;

	horizontal_scroll_bar(horizontal_scroll_bar&&) = delete;
	horizontal_scroll_bar& operator=(horizontal_scroll_bar&&) = delete;

	~horizontal_scroll_bar() override = default;
};

} // namespace ruis
