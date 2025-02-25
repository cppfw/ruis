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

// suppress lint, because "std::shared_from_this is public via fraction_band_widget"
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this)
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
inline utki::shared_ref<ruis::scroll_bar> scroll_bar(
	utki::shared_ref<context> context, //
	ruis::scroll_bar::all_parameters params
)
{
	return utki::make_shared<ruis::scroll_bar>(
		std::move(context), //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
