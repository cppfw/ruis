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

#include "nine_patch.hpp"

#include <utki/types.hpp>
#include <utki/util.hpp>

#include "../../context.hpp"
#include "../../layout/linear_layout.hpp"
#include "../../layout/pile_layout.hpp"
#include "../../util/util.hpp"

using namespace std::string_literals;

using namespace ruis;

namespace {
namespace m {
using namespace ruis::make;
} // namespace m
} // namespace

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_widget_structure(
	utki::shared_ref<ruis::context> c,
	container::parameters container_params,
	std::array<std::array<utki::shared_ref<ruis::image>, 3>, 3> image_widgets_matrix
)
{
	// clang-format off
	return {
		m::container(c,
			{
				.layout_params = {
					.dims = {dim::fill, dim::min}
				},
				.container_params = {
					.layout = layout::row
				}
			},
			{
				std::move(image_widgets_matrix[0][0]),
				std::move(image_widgets_matrix[0][1]),
				std::move(image_widgets_matrix[0][2])
			}
		),
		m::container(c,
			{
				.layout_params = {
					.dims = {dim::max, dim::max},
					.weight = 1
				},
				.container_params = {
					.layout = layout::row
				}
			},
			{
				std::move(image_widgets_matrix[1][0]),
				m::pile(c,
					{
						.layout_params = {
							.dims = {dim::max, dim::max},
							.weight = 1
						}
					},
					{
						std::move(image_widgets_matrix[1][1]),
						m::container(c,
							{
								.layout_params = {
									.dims = {dim::max, dim::max}
								},
								.widget_params = {
									.id = "ruis_content"s
								},
								.container_params = std::move(container_params)
							}
						)
					}
				),
				std::move(image_widgets_matrix[1][2]),
			}
		),
		m::container(c,
			{
				.layout_params = {
					.dims = {dim::fill, dim::min}
				},
				.container_params = {
					.layout = layout::row
				}
			},
			{
				std::move(image_widgets_matrix[2][0]),
				std::move(image_widgets_matrix[2][1]),
				std::move(image_widgets_matrix[2][2])
			}
		)
	};
	// clang-format on
}
} // namespace

nine_patch::nine_patch(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	// clang-format off
	widget(c, desc),
	blending_widget(this->context, desc),
	frame_widget(
		this->context,
		{
			.layout = layout::column
		},
		{},
		make_widget_structure(
			this->context,
			{
				.layout = ruis::layout::pile
			},
			{{
				{{
					m::image(c,
						{
							.widget_params = {
								.id = "ruis_lt"s
							}
						}
					),
					m::image(c,
						{
							.layout_params = {
								.dims = {dim::fill, dim::min},
								.weight = 1
							},
							.widget_params = {
								.id = "ruis_t"s
							}
						}
					),
					m::image(c,
						{
							.widget_params = {
								.id = "ruis_rt"s
							}
						}
					)
				}},
				{{
					m::image(c,
						{
							.layout_params = {
								.dims = {dim::min, dim::fill}
							},
							.widget_params = {
								.id = "ruis_l"s
							}
						}
					),
					m::image(c,
						{
							.layout_params = {
								.dims = {dim::fill, dim::fill}
							},
							.widget_params = {
								.id = "ruis_m"s
							}
						}
					),
					m::image(c,
						{
							.layout_params = {
								.dims = {dim::min, dim::fill}
							},
							.widget_params = {
								.id = "ruis_r"s
							}
						}
					)
				}},
				{{
					m::image(c,
						{
							.widget_params = {
								.id = "ruis_lb"s
							}
						}
					),
					m::image(c,
						{
							.layout_params = {
								.dims = {dim::fill, dim::min},
								.weight = 1
							},
							.widget_params = {
								.id = "ruis_b"s
							}
						}
					),
					m::image(c,
						{
							.widget_params = {
								.id = "ruis_rb"s
							}
						}
					)
				}}
			}}
		),
		{}
	),
	img_widgets_matrix(
		{{
			{
				this->get_widget_as<image>("ruis_lt"),
				this->get_widget_as<image>("ruis_t"),
				this->get_widget_as<image>("ruis_rt")
			},
			{
				this->get_widget_as<image>("ruis_l"),
				this->get_widget_as<image>("ruis_m"),
				this->get_widget_as<image>("ruis_r")
			},
			{
				this->get_widget_as<image>("ruis_lb"),
				this->get_widget_as<image>("ruis_b"),
				this->get_widget_as<image>("ruis_rb")
			}
		}}
	)
// clang-format on
{
	this->nine_patch::on_blending_change();

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		// if (p.value == "left") {
		// 	// 'min' is by default, but not allowed to specify explicitly, as well as 'max' and 'fill',
		// 	// thus we can use parse_dimension_value().
		// 	this->params.borders.left() =
		// 		parse_dimension_value(get_property_value(p), this->context.get().units).get(this->context);
		// } else if (p.value == "right") {
		// 	this->params.borders.right() =
		// 		parse_dimension_value(get_property_value(p), this->context.get().units).get(this->context);
		// } else if (p.value == "top") {
		// 	this->params.borders.top() =
		// 		parse_dimension_value(get_property_value(p), this->context.get().units).get(this->context);
		// } else if (p.value == "bottom") {
		// 	this->params.borders.bottom() =
		// 		parse_dimension_value(get_property_value(p), this->context.get().units).get(this->context);
		// } else if (p.value == "center_visible") {
		// 	this->set_center_visible(get_property_value(p).to_bool());
		// }
	}

	// this should go after setting up border widgets
	{
		auto i = std::find(desc.begin(), desc.end(), "image");
		if (i != desc.end()) {
			this->set_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(*i).string).to_shared_ptr()
			);
		}
	}
	{
		auto i = std::find(desc.begin(), desc.end(), "disabled_image");
		if (i != desc.end()) {
			this->set_disabled_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(*i).string).to_shared_ptr()
			);
		}
	}

	this->content().push_back_inflate(desc);
}

nine_patch::nine_patch( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	utki::span<const utki::shared_ref<widget>> children
) :
	nine_patch(
		context,
		std::move(params),
		children,
		// clang-format off
		{{
			{{
				m::image(context,
					{}
				),
				m::image(context,
					{
						.layout_params = {
							.dims = {dim::fill, dim::min},
							.weight = 1
						}
					}
				),
				m::image(context,
					{}
				)
			}},
			{{
				m::image(context,
					{
						.layout_params = {
							.dims = {dim::min, dim::fill}
						}
					}
				),
				m::image(context,
					{
						.layout_params = {
							.dims = {dim::fill, dim::fill}
						}
					}
				),
				m::image(context,
					{
						.layout_params = {
							.dims = {dim::min, dim::fill}
						}
					}
				)
			}},
			{{
				m::image(context,
					{}
				),
				m::image(context,
					{
						.layout_params = {
							.dims = {dim::fill, dim::min},
							.weight = 1
						}
					}
				),
				m::image(context,
					{}
				)
			}}
		}}
		// clang-format on
	)
{}

nine_patch::nine_patch( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	utki::span<const utki::shared_ref<widget>> children,
	std::array<std::array<utki::shared_ref<ruis::image>, 3>, 3> image_widgets_matrix
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	blending_widget( //
		this->context,
		std::move(params.blending_params)
	),
	frame_widget( //
		this->context,
		{.layout = layout::column},
		std::move(params.frame_params),
		make_widget_structure( //
			this->context,
			[&](){
				auto ret = std::move(params.container_params);
				if(!ret.layout){
					// pile layout by default
					ret.layout = ruis::layout::pile;
				}
				return ret;
			}(),
			image_widgets_matrix
		),
		children
	),
	img_widgets_matrix(
		// clang-format off
		{{
			{
				image_widgets_matrix[0][0].get(),
				image_widgets_matrix[0][1].get(),
				image_widgets_matrix[0][2].get()
			},
			{
				image_widgets_matrix[1][0].get(),
				image_widgets_matrix[1][1].get(),
				image_widgets_matrix[1][2].get()
			},
			{
				image_widgets_matrix[2][0].get(),
				image_widgets_matrix[2][1].get(),
				image_widgets_matrix[2][2].get()
			}
		}}
		// clang-format on
	),
	params(std::move(params.nine_patch_params))
{
	this->nine_patch::on_blending_change();

	// this should go after setting up border widgets
	if (this->params.nine_patch) {
		this->set_nine_patch(this->params.nine_patch);
	}
	if (this->params.disabled_nine_patch) {
		this->set_disabled_nine_patch(this->params.disabled_nine_patch);
	}
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
	auto* np = this->params.nine_patch.get();

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		np = this->params.disabled_nine_patch.get();
	}

	sides<real> ret;

	for (size_t i = 0; i != ret.size(); ++i) {
		if (!this->get_borders()[i].is_undefined()) {
			ret[i] = this->get_borders()[i].get(this->context);
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
	auto* np = this->params.nine_patch.get();

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

	{
		// non-const call to get_layout_params requests re-layout which is not necessarily needed, so try to avoid it if
		// possible
		auto& tl_lp = this->img_widgets_matrix[0][0].get().get_layout_params_const();

		if (this->get_borders().left().is_undefined()) { // min
			if (tl_lp.dims.x() != min_borders.left()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.x() = length::make_px(min_borders.left());
			}
		} else {
			if (tl_lp.dims.x() != this->get_borders().left()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.x() = this->get_borders().left();
			}
		}

		if (this->get_borders().top().is_undefined()) { // min
			if (tl_lp.dims.y() != min_borders.top()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.y() = length::make_px(min_borders.top());
			}
		} else {
			if (tl_lp.dims.y() != this->get_borders().top()) {
				auto& lp = this->img_widgets_matrix[0][0].get().get_layout_params();
				lp.dims.y() = this->get_borders().top();
			}
		}
	}
	{
		// non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if
		// possible
		auto& br_lp = this->img_widgets_matrix[2][2].get().get_layout_params_const();

		if (this->get_borders().right().is_undefined()) { // min
			if (br_lp.dims.x() != min_borders.right()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.x() = length::make_px(min_borders.right());
			}
		} else {
			if (br_lp.dims.x() != this->get_borders().right()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.x() = this->get_borders().right();
			}
		}

		if (this->get_borders().bottom().is_undefined()) { // min
			if (br_lp.dims.y() != min_borders.bottom()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.y() = length::make_px(min_borders.bottom());
			}
		} else {
			if (br_lp.dims.y() != this->get_borders().bottom()) {
				auto& lp = this->img_widgets_matrix[2][2].get().get_layout_params();
				lp.dims.y() = this->get_borders().bottom();
			}
		}
		//			TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
	//		TRACE(<< "this->borders = " << this->borders << std::endl)

	this->img_res_matrix = np->get(this->get_borders());

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
			this->img_widgets_matrix[i][j].get().set_blending_factors(this->get_blending_factors());
		}
	}
}

void nine_patch::on_enabled_change()
{
	this->frame_widget::on_enabled_change();

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

void nine_patch::on_borders_change()
{
	this->apply_images();
	this->invalidate_layout();
}
