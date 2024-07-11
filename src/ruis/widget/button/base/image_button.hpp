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

#pragma once

#include "../../../res/image.hpp"
#include "../../label/image.hpp"

#include "button.hpp"

namespace ruis {
class image_button :
	virtual public button, //
	public image // TODO: why public?
{
public:
	struct parameters {
		std::shared_ptr<const res::image> unpressed_image;
		std::shared_ptr<const res::image> pressed_image;
	};

private:
	parameters params;

	void update_image();

protected:
	void on_pressed_change() override;

	image_button(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

public:
	void set_pressed_image(std::shared_ptr<const res::image> image);

	const decltype(params.pressed_image)& get_pressed_image() const
	{
		return this->params.pressed_image;
	}

	void set_unpressed_image(std::shared_ptr<const res::image> image);

	const decltype(params.unpressed_image)& get_unpressed_image() const
	{
		return this->params.unpressed_image;
	}
};
} // namespace ruis
