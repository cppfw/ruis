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

#include "../../res/image.hpp"

#include "choice_group.hpp"

namespace ruis {

class tab_group : virtual public widget, public choice_group
{
	std::shared_ptr<res::image> filler;
	std::shared_ptr<const res::image::texture> filler_texture;

public:
	tab_group(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc);

	tab_group(const tab_group&) = delete;
	tab_group& operator=(const tab_group&) = delete;

	tab_group(tab_group&&) = delete;
	tab_group& operator=(tab_group&&) = delete;

	~tab_group() override = default;

	void set_filler(std::shared_ptr<res::image> filler);

	ruis::vector2 measure(const ruis::vector2& quotum) const override;

	void on_lay_out() override;

	void render(const ruis::matrix4& matrix) const override;
};

} // namespace ruis
