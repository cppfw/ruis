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

#include "gradient.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

gradient::gradient( //
	utki::shared_ref<ruis::context> context,
	all_parameters params
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	params(std::move(params.gradient_params))
{}

void gradient::render(const matrix4& matrix) const
{
	this->context.get().renderer.get().set_simple_alpha_blending();

	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	if (this->params.gradient) {
		//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		this->params.gradient->render(matr);
	}
}
