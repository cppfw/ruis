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

#include "nine_patch.hpp"

#include <utki/types.hpp>
#include <utki/util.hpp>

#include "../../context.hpp"
#include "../../layouts/linear_layout.hpp"
#include "../../make.hpp"
#include "../../util/util.hpp"

using namespace ruis;

namespace {
std::vector<utki::shared_ref<ruis::widget>> build_layout(utki::shared_ref<ruis::context> c)
{
	namespace m = ruis::make;
	using ruis::lp;

	// clang-format off
	return {
		m::container(c,
			{
				.lp = {
					.dims = {lp::fill, lp::min}
				}
			},
			{row_layout::instance},
			{
			m::image(c, {.id = "ruis_lt"}),
			m::image(c,
				{
					.id = "ruis_t",
					.lp = {
						.dims = {0, lp::min},
						.weight = 1
					}
				}),
			m::image(c, {.id = "ruis_rt"})
			}
		),
		m::container(c,
			{
				.lp = {
					.dims = {lp::max, lp::min},
					.weight = 1
				}
			},
			{row_layout::instance},
			{
			m::image(c,
				{
					.id = "ruis_l",
					.lp = {
						.dims = {lp::min, lp::fill}
					}
				}),
			m::container(c,
				{
					.id = "ruis_content",
					.lp = {
						.dims = {lp::min, lp::max},
						.weight = 1
					}
				},
				{pile_layout::instance},
				{
					m::image(c,
						{
							.id = "ruis_m",
							.lp = {
								.dims = {lp::fill, lp::fill}
							}
						})
				}),
			m::image(c,
				{
					.id = "ruis_r",
					.lp = {
						.dims = {lp::min, lp::fill}
					}
				})
			}
		),
		m::container(c,
			{
				.lp = {
					.dims = {lp::fill, lp::min}
				}
			},
			{row_layout::instance},
			{
			m::image(c, {.id = "ruis_lb"}),
			m::image(c,
				{
					.id = "ruis_b",
					.lp = {
						.dims = {0, lp::min},
						.weight = 1
					}
				}),
			m::image(c, {.id = "ruis_rb"})
			})
	};
	// clang-format on
}
} // namespace

nine_patch::nine_patch(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	blending_widget(this->context, desc),
	container(
		this->context,
		{
},
		{column_layout::instance},
		build_layout(this->context)
	),
	img_widgets_matrix({{//
						 {//
						  this->get_widget_as<image>("ruis_lt"),
						  this->get_widget_as<image>("ruis_t"),
						  this->get_widget_as<image>("ruis_rt")
						 }, //
						 {//
						  this->get_widget_as<image>("ruis_l"),
						  this->get_widget_as<image>("ruis_m"),
						  this->get_widget_as<image>("ruis_r")
						 }, //
						 {//
						  this->get_widget_as<image>("ruis_lb"),
						  this->get_widget_as<image>("ruis_b"),
						  this->get_widget_as<image>("ruis_rb")
						 }
	}}),
	inner_content(this->get_widget_as<container>("ruis_content"))
{
	this->nine_patch::on_blending_change();

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "left") {
			// 'min' is by default, but not allowed to specify explicitly, as well as 'max' and 'fill',
			// thus we can use parse_dimension_value().
			this->params.borders.left() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "right") {
			this->params.borders.right() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "top") {
			this->params.borders.top() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "bottom") {
			this->params.borders.bottom() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "center_visible") {
			this->set_center_visible(get_property_value(p).to_bool());
		}
	}

	// this should go after setting up border widgets
	{
		auto i = std::find(desc.begin(), desc.end(), "image");
		if (i != desc.end()) {
			this->set_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(*i).to_string()).to_shared_ptr()
			);
		}
	}
	{
		auto i = std::find(desc.begin(), desc.end(), "disabled_image");
		if (i != desc.end()) {
			this->set_disabled_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(*i).to_string()).to_shared_ptr()
			);
		}
	}

	this->inner_content.push_back_inflate(desc);
}

nine_patch::nine_patch(
	utki::shared_ref<ruis::context> context,
	widget::parameters widget_params,
	blending_widget::parameters blending_widget_params,
	parameters params,
	utki::span<const utki::shared_ref<widget>> children
) :
	widget(std::move(context), std::move(widget_params)),
	blending_widget(this->context, std::move(blending_widget_params)),
	container(
		this->context,
		{
},
		{column_layout::instance},
		build_layout(this->context)
	),
	img_widgets_matrix({{//
						 {//
						  this->get_widget_as<image>("ruis_lt"),
						  this->get_widget_as<image>("ruis_t"),
						  this->get_widget_as<image>("ruis_rt")
						 }, //
						 {//
						  this->get_widget_as<image>("ruis_l"),
						  this->get_widget_as<image>("ruis_m"),
						  this->get_widget_as<image>("ruis_r")
						 }, //
						 {//
						  this->get_widget_as<image>("ruis_lb"),
						  this->get_widget_as<image>("ruis_b"),
						  this->get_widget_as<image>("ruis_rb")
						 }
	}}),
	inner_content(this->get_widget_as<container>("ruis_content")), params(std::move(params))
{
	this->inner_content.push_back(children);
}

void nine_patch::render(const ruis::matrix4& matrix) const
{
	this->container::render(matrix);
}

void nine_patch::set_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.nine_patch = std::move(np);

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		return;
	}

	this->update_images();
}

void nine_patch::set_disabled_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.disabled_nine_patch = std::move(np);

	if (this->is_enabled()) {
		return;
	}

	this->update_images();
}

sides<real> nine_patch::get_actual_borders() const noexcept
{
	auto np = this->params.nine_patch.get();

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		np = this->params.disabled_nine_patch.get();
	}

	sides<real> ret;

	for (size_t i = 0; i != ret.size(); ++i) {
		if (this->params.borders[i] >= 0) {
			ret[i] = this->params.borders[i];
		} else if (!np) {
			ret[i] = 0;
		} else {
			ret[i] = np->get_borders()[i];
		}
	}

	return ret;
}

void nine_patch::apply_images()
{
	auto np = this->params.nine_patch.get();

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		np = this->params.disabled_nine_patch.get();
	}

	if (!np) {
		for (auto& i : this->img_widgets_matrix) {
			for (auto& j : i) {
				j.get().set_image(nullptr);
			}
		}
		return;
	}

	ASSERT(np)
	auto& min_borders = np->get_borders();
	//		TRACE(<< "min_borders = " << min_borders << std::endl)

	{
		// non-const call to get_layout_params requests re-layout which is not necessarily needed, so try to avoid it if
		// possible
		auto& tl_lp = this->img_widgets_matrix[0][0].get().get_layout_params_const();

		if (this->params.borders.left() == lp::min) {
			if (tl_lp.dims.x() != min_borders.left()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.x() = min_borders.left();
			}
		} else {
			if (tl_lp.dims.x() != this->params.borders.left()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.x() = this->params.borders.left();
			}
		}

		if (this->params.borders.top() == lp::min) {
			if (tl_lp.dims.y() != min_borders.top()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.y() = min_borders.top();
			}
		} else {
			if (tl_lp.dims.y() != this->params.borders.top()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.y() = this->params.borders.top();
			}
		}
		//			TRACE(<< "tl_lp.dim = " << tl_lp.dim << std::endl)
	}
	{
		// non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if
		// possible
		auto& br_lp = this->img_widgets_matrix[2][2].get().get_layout_params_const();

		if (this->params.borders.right() == lp::min) {
			if (br_lp.dims.x() != min_borders.right()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.x() = min_borders.right();
			}
		} else {
			if (br_lp.dims.x() != this->params.borders.right()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.x() = this->params.borders.right();
			}
		}

		if (this->params.borders.bottom() == lp::min) {
			if (br_lp.dims.y() != min_borders.bottom()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.y() = min_borders.bottom();
			}
		} else {
			if (br_lp.dims.y() != this->params.borders.bottom()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.y() = this->params.borders.bottom();
			}
		}
		//			TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
	//		TRACE(<< "this->borders = " << this->borders << std::endl)

	this->img_res_matrix = np->get(this->params.borders);

	for (unsigned i = 0; i != 3; ++i) {
		for (unsigned j = 0; j != 3; ++j) {
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
			this->img_widgets_matrix[i][j].get().set_image(this->img_res_matrix->images()[i][j].to_shared_ptr());
		}
	}
}

void nine_patch::set_center_visible(bool visible)
{
	this->img_widgets_matrix[1][1].get().set_visible(visible);
}

void nine_patch::on_blending_change()
{
	for (unsigned i = 0; i != 3; ++i) {
		for (unsigned j = 0; j != 3; ++j) {
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
			this->img_widgets_matrix[i][j].get().set_blending_params(this->get_blending_params());
		}
	}
}

void nine_patch::on_enabled_change()
{
	this->container::on_enabled_change();

	if (!this->params.disabled_nine_patch) {
		// there is no disabled nine patch, so nothing changes
		return;
	}

	this->update_images();
}

void nine_patch::update_images()
{
	this->img_res_matrix.reset();
	this->apply_images();
	this->clear_cache();
}
