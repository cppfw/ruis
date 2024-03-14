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

#include "spinner.hpp"

#include <ratio>

#include <utki/math.hpp>

using namespace ruis;

spinner::spinner(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	image(this->context, desc)
{}

spinner::spinner(
	utki::shared_ref<ruis::context> context,
	widget::parameters widget_params,
	blending_widget::parameters blending_widget_params,
	image::parameters image_params
) :
	widget(std::move(context), widget::all_parameters{std::move(widget_params)}),
	image(this->context, {}, std::move(blending_widget_params), std::move(image_params))
{}

void spinner::render(const matrix4& matrix) const
{
	matrix4 matr(matrix);

	matr.translate(this->rect().d / 2);
	matr.rotate(this->angle);
	matr.translate(-this->rect().d / 2);

	this->image::render(matr);
}

void spinner::set_active(bool active)
{
	if (active) {
		this->context.get().updater.get().start(utki::make_shared_from(*this).to_shared_ptr());
	} else {
		this->context.get().updater.get().stop(*this);
	}
}

void spinner::update(uint32_t dt_ms)
{
	angle += real(utki::pi) / real(std::milli::den) * real(dt_ms);
}
