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

#include "standard_widgets.hpp"

#include "res/tml.hpp"

using namespace std::string_view_literals;

using namespace ruis;

void ruis::init_standard_widgets(
	ruis::context& context, //
	const fsif::file& fi,
	theme th
)
{
	// mount default resource pack

	std::vector<std::string> paths;

	if (!fi.path().empty()) {
		paths.push_back(fi.path());
	}

	paths.emplace_back("ruis_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || (M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
		;

	paths.push_back(utki::cat("/usr/local/share/ruis/res"sv, soname));
	paths.push_back(utki::cat("/usr/share/ruis/res"sv, soname));
#endif

	bool mounted = false;
	for (const auto& s : paths) {
		try {
			fi.set_path(s);
			context.loader().mount_res_pack(fi);
		} catch (std::runtime_error&) {
			continue;
		}

		mounted = true;
		break;
	}

	if (!mounted) {
		throw std::runtime_error("init_standard_widgets(): could not mount default resource pack");
	}

	auto theme_resource_id = [&]() {
		switch (th) {
			using enum theme;
			default:
			case dark:
				return "ruis_tml_theme_dark"sv;
			case light:
				return "ruis_tml_theme_light"sv;
		}
	}();

	auto style_res = context.loader().load<ruis::res::tml>(theme_resource_id);
	context.style().set(utki::make_shared<style_sheet>(style_res.get().forest));
}
