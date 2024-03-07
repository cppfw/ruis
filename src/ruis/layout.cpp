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

#include "layout.hpp"

#include "layouts/linear_layout.hpp"
#include "layouts/pile_layout.hpp"
#include "layouts/size_layout.hpp"
#include "layouts/trivial_layout.hpp"

using namespace ruis;

const utki::shared_ref<layout> layout::trivial = utki::make_shared<trivial_layout>();
const utki::shared_ref<layout> layout::size = utki::make_shared<size_layout>();
const utki::shared_ref<layout> layout::pile = utki::make_shared<pile_layout>();
const utki::shared_ref<layout> layout::row = utki::make_shared<linear_layout>(false);
const utki::shared_ref<layout> layout::column = utki::make_shared<linear_layout>(true);
