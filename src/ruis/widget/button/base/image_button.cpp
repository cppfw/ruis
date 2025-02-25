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

#include "image_button.hpp"

#include "../../../util/util.hpp"

using namespace ruis;

void image_button::update_image()
{
	if (this->is_pressed()) {
		this->set_image(this->params.pressed_image);
	} else {
		this->set_image(this->params.unpressed_image);
	}
}

void image_button::on_pressed_change()
{
	this->update_image();
	this->button::on_pressed_change();
}

image_button::image_button( //
	utki::shared_ref<ruis::context> context,
	blending_widget::parameters blending_params,
	image::parameters image_params,
	parameters params
) :
	widget(std::move(context), {}, {}),
	button(this->context, button::parameters{}),
	image( //
		this->context,
		{.blending_params = std::move(blending_params), //
		 .image_params = std::move(image_params)}
	),
	params(std::move(params))
{
	this->update_image();
}

void image_button::set_pressed_image(std::shared_ptr<const res::image> image)
{
	this->params.pressed_image = std::move(image);
	this->update_image();
}

void image_button::set_unpressed_image(std::shared_ptr<const res::image> image)
{
	this->params.unpressed_image = std::move(image);
	this->update_image();
}
