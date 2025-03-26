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

#include "texture_cube.hpp"

#include "../util/util.hpp"

using namespace std::string_view_literals;

using namespace ruis::res;

texture_cube::texture_cube(utki::shared_ref<const render::texture_cube> tex) :
	tex_cube(std::move(tex))
{}

namespace {
std::string_view check_not_empty(
	std::string_view v, //
	std::string_view field_name
)
{
	if (!v.empty()) {
		return v;
	}

	throw std::logic_error(utki::cat("res::texture_cube::load(): ", field_name, " is not given"));
}

constexpr auto file_px_param = "file_px"sv;
constexpr auto file_nx_param = "file_nx"sv;
constexpr auto file_py_param = "file_py"sv;
constexpr auto file_ny_param = "file_ny"sv;
constexpr auto file_pz_param = "file_pz"sv;
constexpr auto file_nz_param = "file_nz"sv;
} // namespace

utki::shared_ref<texture_cube> texture_cube::load(
	const ruis::resource_loader& loader,
	const tml::forest& desc,
	const papki::file& fi
)
{
	std::string_view file_px;
	std::string_view file_nx;
	std::string_view file_py;
	std::string_view file_ny;
	std::string_view file_pz;
	std::string_view file_nz;

	for (auto& p : desc) {
		if (p.value == file_px_param) {
			file_px = get_property_value(p).string;
		} else if (p.value == file_nx_param) {
			file_nx = get_property_value(p).string;
		} else if (p.value == file_py_param) {
			file_py = get_property_value(p).string;
		} else if (p.value == file_ny_param) {
			file_ny = get_property_value(p).string;
		} else if (p.value == file_pz_param) {
			file_pz = get_property_value(p).string;
		} else if (p.value == file_nz_param) {
			file_nz = get_property_value(p).string;
		}
	}

	auto tex = loader.renderer.get().render_context.get().create_texture_cube( //
		rasterimage::read(fi.set_path(check_not_empty(file_px, file_px_param))),
		rasterimage::read(fi.set_path(check_not_empty(file_nx, file_nx_param))),
		rasterimage::read(fi.set_path(check_not_empty(file_py, file_py_param))),
		rasterimage::read(fi.set_path(check_not_empty(file_ny, file_ny_param))),
		rasterimage::read(fi.set_path(check_not_empty(file_pz, file_pz_param))),
		rasterimage::read(fi.set_path(check_not_empty(file_nz, file_nz_param)))
	);

	return utki::make_shared<texture_cube>(std::move(tex));
}
