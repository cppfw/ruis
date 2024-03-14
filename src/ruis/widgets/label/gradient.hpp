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

#include "../../res/gradient.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Gradient widget.
 * This is a widget which can display a rectangle with gradient.
 * From GUI script it can be instantiated as "gradient".
 *
 * @param gradient - gradient resource name.
 */
class gradient : public widget
{
public:
	struct parameters {
		std::shared_ptr<res::gradient> gradient;
	};

private:
	parameters params;

public:
	gradient(utki::shared_ref<ruis::context> context, widget::parameters widget_params, parameters params);

	gradient(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	gradient(const gradient&) = delete;
	gradient& operator=(const gradient&) = delete;

	gradient(gradient&&) = delete;
	gradient& operator=(gradient&&) = delete;

	~gradient() override = default;

	void render(const ruis::matrix4& matrix) const override;
};

namespace make {
[[deprecated]]
inline utki::shared_ref<ruis::widget> gradient(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::gradient::parameters params
)
{
	return utki::make_shared<ruis::gradient>(std::move(context), std::move(widget_params), std::move(params));
}

struct gradient_parameters {
	widget::parameters widget_params;
	ruis::gradient::parameters gradient_params;
};

inline utki::shared_ref<ruis::widget> gradient(utki::shared_ref<ruis::context> context, gradient_parameters params)
{
	return utki::make_shared<ruis::gradient>(
		std::move(context),
		std::move(params.widget_params),
		std::move(params.gradient_params)
	);
}

} // namespace make

} // namespace ruis
