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

#include "spinning_image.hpp"

#include <ratio>

#include <utki/math.hpp>

using namespace std::string_view_literals;

using namespace ruis;

spinning_image::spinning_image( //
	const utki::shared_ref<ruis::context>& context,
	all_parameters params
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	image(context, {.params = std::move(params.params.image)}),
	params(std::move(params.params.specific))
{}

void spinning_image::render(const mat4& matrix) const
{
	mat4 matr(matrix);

	matr.translate(this->rect().d / 2);
	matr.rotate(ruis::quat(this->angle));
	matr.translate(-this->rect().d / 2);

	this->image::render(matr);
}

void spinning_image::set_active(bool active)
{
	if (active) {
		this->context.get().updater.get().start(utki::make_shared_from(*this));
	} else {
		this->context.get().updater.get().stop(*this);
	}
}

void spinning_image::update(uint32_t dt_ms)
{
	angle += this->params.rounds_per_second / real(std::milli::den) * real(dt_ms);
}

utki::shared_ref<ruis::spinning_image> ruis::make::refresh(
	const utki::shared_ref<ruis::context>& context, //
	spinning_image::all_parameters params
)
{
	if (auto& src = params.params.image.specific.source; !src) {
		src = context.get().loader().load<res::image>("ruis_img_refresh"sv);
	}
	if (auto& im = params.params.image.specific.disabled_img; !im) {
		im = context.get().loader().load<res::image>("ruis_img_refresh_disabled"sv);
	}

	return utki::make_shared<ruis::spinning_image>(
		context, //
		std::move(params)
	);
}
