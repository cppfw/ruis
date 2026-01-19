/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

using namespace ruis;

image::image( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
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
	params(std::move(params.image_params)),
	vao(this->context.get().renderer.get().obj().empty_vertex_array)
{}

void image::render(const ruis::mat4& matrix) const
{
	auto img = this->params.img.get();

	if (!this->is_enabled() && this->params.disabled_img) {
		img = this->params.disabled_img.get();
	}

	if (!img) {
		return;
	}

	this->set_blending_to_renderer();

	auto& r = this->context.get().renderer.get();

	if (!this->texture) {
		this->texture = img->get(this->context.get().units, this->rect().d).to_shared_ptr();
		this->vao = this->context.get().renderer.get().obj().pos_tex_quad_01_vao;
	}
	ASSERT(this->texture)

	ruis::mat4 matr(matrix);
	matr.scale(this->rect().d);

	r.render(
		matr, //
		*this->texture,
		this->vao
	);
}

ruis::vec2 image::measure(const ruis::vec2& quotum) const
{
	auto img = this->params.img.get();

	if (!this->is_enabled() && this->params.disabled_img) {
		img = this->params.disabled_img.get();
	}

	if (!img) {
		return {0, 0};
	}

	vec2 img_dims = img->dims(this->context.get().units).to<real>();

	ASSERT(img_dims.is_positive_or_zero(), [&](auto& o) {
		o << "img_dims = " << img_dims << " widget id = " << this->id();
	})

	if (!this->params.keep_aspect_ratio) {
		vec2 ret = img_dims;

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

	using std::round;

	if (quotum.x() < 0 && quotum.y() < 0) {
		return img_dims;
	} else if (quotum.x() < 0) {
		ASSERT(quotum.y() >= 0)

		vec2 ret = {
			round(ratio * quotum.y()), //
			quotum.y()
		};
		return ret;
	} else if (quotum.y() >= 0) {
		ASSERT(quotum.x() >= 0)
		ASSERT(quotum.y() >= 0)
		// This case is possible when image layout parameters are, for example 'layout dims = {max, fill}', so the
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

		vec2 ret = {
			quotum.x(), //
			round(quotum.x() / ratio)
		};
		return ret;
	}
}

void image::set_image(std::shared_ptr<const res::image> image)
{
	auto& c = this->context.get();
	if (this->params.img && image && this->params.img->dims(c.units) == image->dims(c.units)) {
	} else {
		this->invalidate_layout();
	}

	this->params.img = std::move(image);
	this->texture.reset();
}

void image::set_disabled_image(std::shared_ptr<const res::image> image)
{
	auto& c = this->context.get();
	if (this->params.disabled_img && image && this->params.disabled_img->dims(c.units) == image->dims(c.units)) {
	} else {
		if (!this->is_enabled()) {
			this->invalidate_layout();
		}
	}

	this->params.disabled_img = std::move(image);
	this->texture.reset();
}

void image::on_resize()
{
	this->widget::on_resize();
	this->texture.reset();
}

void image::on_enabled_change()
{
	if (!this->params.disabled_img) {
		// no disabled image set, nothing changes
		return;
	}

	this->texture.reset();
	this->clear_cache();

	if (this->params.img) {
		auto& c = this->context.get();

		// if dimension of active image change then need to re-layout
		if (this->params.disabled_img->dims(c.units) != this->params.img->dims(c.units)) {
			this->invalidate_layout();
		}
	} else {
		// there is only disabled image, perhaps nobody will ever use it like this, but nevertheless,
		// let's handle this case gracefully
		this->invalidate_layout();
	}
}
