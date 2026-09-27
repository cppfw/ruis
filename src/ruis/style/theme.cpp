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

#include "theme.hpp"

#include "../res/tml.hpp"

using namespace std::string_view_literals;

using namespace ruis;

std::string_view ruis::to_resource_id(theme th) noexcept
{
	switch (th) {
		using enum theme;
		case dark:
			return "ruis_tml_theme_dark"sv;
		case light:
			return "ruis_tml_theme_light"sv;
	}

	return "ruis_tml_theme_dark"sv;
}

utki::shared_ref<style_sheet> ruis::load_theme(
	const resource_loader& loader, //
	std::string_view theme_tml_resource_id
)
{
	return utki::make_shared<style_sheet>(loader.load<ruis::res::tml>(theme_tml_resource_id).get().forest);
}

utki::shared_ref<style_sheet> ruis::load_theme(
	const resource_loader& loader, //
	theme th
)
{
	return load_theme(loader, to_resource_id(th));
}
