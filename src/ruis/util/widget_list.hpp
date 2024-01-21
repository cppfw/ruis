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

#pragma once

#include <vector>

#include <utki/shared_ref.hpp>

namespace morda {

class widget;

using widget_list = std::vector<utki::shared_ref<widget>>;
using semiconst_widget_list = const std::vector<utki::shared_ref<widget>>;
using const_widget_list = const std::vector<utki::shared_ref<const widget>>;
static_assert(
	sizeof(widget_list) == sizeof(const_widget_list),
	"sizeof(widget_list) differs from sizeof(const_widget_list)"
);
static_assert(
	sizeof(widget_list) == sizeof(semiconst_widget_list),
	"sizeof(widget_list) differs from sizeof(semiconst_widget_list)"
);
static_assert(
	sizeof(widget_list::value_type) == sizeof(const_widget_list::value_type),
	"sizeof(widget_list::value_type) differs from sizeof(const_widget_list::value_type)"
);
static_assert(
	sizeof(widget_list::value_type) == sizeof(semiconst_widget_list::value_type),
	"sizeof(widget_list::value_type) differs from sizeof(semiconst_widget_list::value_type)"
);

} // namespace morda
