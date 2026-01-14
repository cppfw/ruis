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

#include "texture_font_provider.hxx"

using namespace ruis;

texture_font_provider::texture_font_provider(
	utki::shared_ref<const ruis::render::context> rendering_context, //
	utki::shared_ref<const ruis::render::renderer::objects> common_rendering_objects,
	utki::shared_ref<const freetype_face> face,
	unsigned max_cached
) :
	rendering_context(std::move(rendering_context)),
	common_rendering_objects(std::move(common_rendering_objects)),
	face(std::move(face)),
	max_cached(max_cached)
{}

utki::shared_ref<const font> texture_font_provider::create(real size) const
{
	return utki::make_shared<texture_font>(
		this->rendering_context, //
		this->common_rendering_objects,
		this->face,
		unsigned(size),
		max_cached
	);
}
