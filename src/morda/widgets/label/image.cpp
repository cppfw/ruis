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

#include "image.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace morda;

image::image(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	blending_widget(this->context, desc),
	vao(this->context.get().renderer.get().empty_vertex_array)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "image") {
			this->img = this->context.get().loader.load<res::image>(get_property_value(p).to_string()).to_shared_ptr();
		} else if (p.value == "disabled_image") {
			this->disabled_img =
				this->context.get().loader.load<res::image>(get_property_value(p).to_string()).to_shared_ptr();
		} else if (p.value == "keep_aspect_ratio") {
			this->keep_aspect_ratio = get_property_value(p).to_bool();
		} else if (p.value == "repeat_x") {
			this->repeat_v.x() = get_property_value(p).to_bool();
		} else if (p.value == "repeat_y") {
			this->repeat_v.y() = get_property_value(p).to_bool();
		}
	}
}

namespace {
const std::array<r4::vector2<float>, 4> quad_fan_tex_coords = {
	{r4::vector2<float>(0, 0), r4::vector2<float>(1, 0), r4::vector2<float>(1, 1), r4::vector2<float>(0, 1)}
};
} // namespace

void image::render(const morda::matrix4& matrix) const
{
	auto img = this->img.get();

	if (!this->is_enabled() && this->disabled_img) {
		img = this->disabled_img.get();
	}

	if (!img) {
		return;
	}

	this->set_blending_to_renderer();

	auto& r = this->context.get().renderer.get();

	if (!this->texture) {
		this->texture = img->get(this->rect().d).to_shared_ptr();

		if (this->repeat_v.x() || this->repeat_v.y()) {
			std::array<r4::vector2<float>, 4> tex_coords{};
			ASSERT(quad_fan_tex_coords.size() == tex_coords.size())
			auto src = quad_fan_tex_coords.cbegin();
			auto dst = tex_coords.begin();
			auto scale = this->rect().d.comp_div(img->dims());
			if (!this->repeat_v.x()) {
				scale.x() = 1;
			}
			if (!this->repeat_v.y()) {
				scale.y() = 1;
			}
			for (; dst != tex_coords.end(); ++src, ++dst) {
				*dst = src->comp_mul(scale);
			}
			this->vao = r.factory->create_vertex_array(
				{r.quad_01_vbo, r.factory->create_vertex_buffer(utki::make_span(tex_coords))},
				r.quad_indices,
				vertex_array::mode::triangle_fan
			);
		} else {
			this->vao = this->context.get().renderer.get().pos_tex_quad_01_vao;
		}
	}
	ASSERT(this->texture)

	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	this->texture->render(matr, this->vao.get());
}

morda::vector2 image::measure(const morda::vector2& quotum) const
{
	auto img = this->img.get();

	if (!this->is_enabled() && this->disabled_img) {
		img = this->disabled_img.get();
	}

	if (!img) {
		return {0};
	}

	vector2 img_dims = img->dims(this->context.get().units.dots_per_inch);

	ASSERT(img_dims.is_positive_or_zero(), [&](auto& o) {
		o << "img_dims = " << img_dims;
	})

	if (!this->keep_aspect_ratio) {
		vector2 ret = img_dims;

		for (unsigned i = 0; i != ret.size(); ++i) {
			if (quotum[i] >= 0) {
				ret[i] = quotum[i];
			}
		}

		return ret;
	}

	ASSERT(img)
	ASSERT(img_dims.y() > 0)

	real ratio = img_dims.x() / img_dims.y();

	if (quotum.x() < 0 && quotum.y() < 0) {
		return img_dims;
	} else if (quotum.x() < 0) {
		ASSERT(quotum.y() >= 0)

		vector2 ret;
		ret.y() = quotum.y();
		ret.x() = ratio * quotum.y();
		return ret;
	} else if (quotum.y() >= 0) {
		ASSERT(quotum.x() >= 0)
		ASSERT(quotum.y() >= 0)
		// This case is possible when image layout parameters are, for example 'dx{max} dy{fill}', so the
		// minimum x size will be determined to keep aspect ratio, but later, the x size of the image widget can be
		// set to fill all the allowed space, in this case the measure() method will be called with
		// both quotum components to be positive numbers.
		return quotum;
	} else {
		ASSERT(quotum.x() >= 0)
		ASSERT(quotum.y() < 0, [&](auto& o) {
			o << "quotum =" << quotum;
		})

		ASSERT(ratio > 0)

		vector2 ret;
		ret.x() = quotum.x();
		ret.y() = quotum.x() / ratio;
		return ret;
	}
}

void image::set_image(std::shared_ptr<const res::image> image)
{
	if (this->img && image && this->img->dims() == image->dims()) {
	} else {
		this->invalidate_layout();
	}

	this->img = std::move(image);
	this->texture.reset();
}

void image::set_disabled_image(std::shared_ptr<const res::image> image)
{
	if (this->disabled_img && image && this->disabled_img->dims() == image->dims()) {
	} else {
		if (!this->is_enabled()) {
			this->invalidate_layout();
		}
	}

	this->disabled_img = std::move(image);
	this->texture.reset();
}

void image::on_resize()
{
	this->widget::on_resize();
	this->texture.reset();
}

void image::on_enable_change()
{
	if (!this->disabled_img) {
		// no disabled image set, nothing changes
		return;
	}

	this->texture.reset();
	this->clear_cache();

	if (this->img) {
		// if dimension of active image change then need to re-layout
		if (this->disabled_img->dims() != this->img->dims()) {
			this->invalidate_layout();
		}
	} else {
		// there is only disabled image, perhaps nobody will ever use it like this, but nevertheless,
		// let's handle this case gracefully
		this->invalidate_layout();
	}
}
