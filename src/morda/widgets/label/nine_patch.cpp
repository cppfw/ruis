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

#include "nine_patch.hpp"

#include <utki/types.hpp>
#include <utki/util.hpp>

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace morda;

namespace {
const auto ninePatchLayout_c = treeml::read(R"qwertyuiop(
		layout{column}

		@row{
			lp{dx{fill}}
			@image{
				id{morda_lt}
			}

			@image{
				lp{dx{0}weight{1}}
				id{morda_t}
			}

			@image{
				id{morda_rt}
			}
		}

		@row{
			lp{
				dx{max}
				weight{1}
			}

			@image{
				id{morda_l}
				lp{dy{fill}}
			}

			@pile{
				id{morda_content}
				lp{
					weight{1}
					dy{max}
				}

				@image{
					id{morda_m}
					lp{dx{fill}dy{fill}}
				}
			}
			@image{
				id{morda_r}
				lp{dy{fill}}
			}
		}

		@row{
			lp{dx{fill}}
			@image{
				id{morda_lb}
			}

			@image{
				lp{dx{0}weight{1}}
				id{morda_b}
			}

			@image{
				id{morda_rb}
			}
		}
	)qwertyuiop");
} // namespace

nine_patch::nine_patch(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	blending_widget(this->context, desc),
	container(this->context, ninePatchLayout_c),
	img_widgets_matrix([this]() -> decltype(this->img_widgets_matrix) {
		return {
			{{this->try_get_widget_as<image>("morda_lt"),
this->try_get_widget_as<image>("morda_t"),
this->try_get_widget_as<image>("morda_rt")},
			 {this->try_get_widget_as<image>("morda_l"),
             this->try_get_widget_as<image>("morda_m"),
             this->try_get_widget_as<image>("morda_r")},
			 {this->try_get_widget_as<image>("morda_lb"),
             this->try_get_widget_as<image>("morda_b"),
             this->try_get_widget_as<image>("morda_rb")}}
        };
	}()),
	// TODO: use get_widget_as()
	inner_content(this->try_get_widget_as<container>("morda_content"))
{
	this->nine_patch::on_blending_change();

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "left") {
			// 'min' is by default, but not allowed to specify explicitly, as well as 'max' and 'fill',
			// so, use parse_dimension_value().
			this->borders.left() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "right") {
			this->borders.right() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "top") {
			this->borders.top() = parse_dimension_value(get_property_value(p), this->context.get().units);
		} else if (p.value == "bottom") {
			this->borders.bottom() = parse_dimension_value(get_property_value(p), this->context.get().units);
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

	this->inner_content->push_back_inflate(desc);
}

void nine_patch::render(const morda::matrix4& matrix) const
{
	this->container::render(matrix);
}

void nine_patch::set_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->np_res = std::move(np);

	if (!this->is_enabled() && this->disabled_np_res) {
		return;
	}

	this->update_images();
}

void nine_patch::set_disabled_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->disabled_np_res = std::move(np);

	if (this->is_enabled()) {
		return;
	}

	this->update_images();
}

sides<real> nine_patch::get_actual_borders() const noexcept
{
	auto np = this->np_res.get();

	if (!this->is_enabled() && this->disabled_np_res) {
		np = this->disabled_np_res.get();
	}

	sides<real> ret;

	for (size_t i = 0; i != ret.size(); ++i) {
		if (this->borders[i] >= 0) {
			ret[i] = this->borders[i];
		} else if (!np) {
			ret[i] = 0;
		} else {
			ret[i] = np->borders()[i];
		}
	}

	return ret;
}

void nine_patch::apply_images()
{
	auto np = this->np_res.get();

	if (!this->is_enabled() && this->disabled_np_res) {
		np = this->disabled_np_res.get();
	}

	if (!np) {
		for (auto& i : this->img_widgets_matrix) {
			for (auto& j : i) {
				j->set_image(nullptr);
			}
		}
		return;
	}

	ASSERT(np)
	auto& minBorders = np->borders();
	//		TRACE(<< "minBorders = " << minBorders << std::endl)

	{
		// non-const call to get_layout_params requests re-layout which is not necessarily needed, so try to avoid it if
		// possible
		auto& tl_lp = this->img_widgets_matrix[0][0]->get_layout_params_const();

		if (this->borders.left() == layout_params::min) {
			if (tl_lp.dims.x() != minBorders.left()) {
				auto& lp = this->img_widgets_matrix[0][0]->get_layout_params();
				lp.dims.x() = minBorders.left();
			}
		} else {
			if (tl_lp.dims.x() != this->borders.left()) {
				auto& lp = this->img_widgets_matrix[0][0]->get_layout_params();
				lp.dims.x() = this->borders.left();
			}
		}

		if (this->borders.top() == layout_params::min) {
			if (tl_lp.dims.y() != minBorders.top()) {
				auto& lp = this->img_widgets_matrix[0][0]->get_layout_params();
				lp.dims.y() = minBorders.top();
			}
		} else {
			if (tl_lp.dims.y() != this->borders.top()) {
				auto& lp = this->img_widgets_matrix[0][0]->get_layout_params();
				lp.dims.y() = this->borders.top();
			}
		}
		//			TRACE(<< "tl_lp.dim = " << tl_lp.dim << std::endl)
	}
	{
		// non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if
		// possible
		auto& br_lp = this->img_widgets_matrix[2][2]->get_layout_params_const();

		if (this->borders.right() == layout_params::min) {
			if (br_lp.dims.x() != minBorders.right()) {
				auto& lp = this->img_widgets_matrix[2][2]->get_layout_params();
				lp.dims.x() = minBorders.right();
			}
		} else {
			if (br_lp.dims.x() != this->borders.right()) {
				auto& lp = this->img_widgets_matrix[2][2]->get_layout_params();
				lp.dims.x() = this->borders.right();
			}
		}

		if (this->borders.bottom() == layout_params::min) {
			if (br_lp.dims.y() != minBorders.bottom()) {
				auto& lp = this->img_widgets_matrix[2][2]->get_layout_params();
				lp.dims.y() = minBorders.bottom();
			}
		} else {
			if (br_lp.dims.y() != this->borders.bottom()) {
				auto& lp = this->img_widgets_matrix[2][2]->get_layout_params();
				lp.dims.y() = this->borders.bottom();
			}
		}
		//			TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
	//		TRACE(<< "this->borders = " << this->borders << std::endl)

	this->img_res_matrix = np->get(this->borders);

	for (unsigned i = 0; i != 3; ++i) {
		for (unsigned j = 0; j != 3; ++j) {
			this->img_widgets_matrix[i][j]->set_image(this->img_res_matrix->images()[i][j].to_shared_ptr());
		}
	}
}

void nine_patch::set_center_visible(bool visible)
{
	ASSERT(this->img_widgets_matrix[1][1])
	this->img_widgets_matrix[1][1]->set_visible(visible);
}

void nine_patch::on_blending_change()
{
	for (unsigned i = 0; i != 3; ++i) {
		for (unsigned j = 0; j != 3; ++j) {
			this->img_widgets_matrix[i][j]->set_blending_params(this->get_blending_params());
		}
	}
}

void nine_patch::on_enable_change()
{
	this->container::on_enable_change();

	if (!this->disabled_np_res) {
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
