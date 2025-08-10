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

#include <memory>

#include <svgren/render.hpp>

// Some bad stuff defines OVERFLOW macro and there is an enum value with same name in svgdom/dom.h.
#ifdef OVERFLOW
#	undef OVERFLOW
#endif

#include <svgdom/dom.hpp>

#include "../context.hpp"
#include "../util/util.hpp"

#include "image.hpp"

using namespace ruis;
using namespace ruis::res;

namespace {
class res_raster_image : public image
{
	utki::shared_ref<const render::texture_2d> tex_2d;

public:
	res_raster_image( //
		utki::shared_ref<const render::texture_2d> tex_2d
	) :
		tex_2d(std::move(tex_2d))
	{}

	utki::shared_ref<const render::texture_2d> get(
		const ruis::units& units, //
		vector2 for_dims
	) const override
	{
		return this->tex_2d;
	}

	r4::vector2<uint32_t> dims(const ruis::units& units) const noexcept override
	{
		return this->tex_2d.get().dims();
	}

	static utki::shared_ref<res_raster_image> load( //
		const ruis::resource_loader& loader,
		const papki::file& fi
	)
	{
		return utki::make_shared<res_raster_image>(loader.renderer.get().render_context.get().make_texture_2d(
			rasterimage::read(fi),
			{
				// TODO: what about params?
			}
		));
	}
};

class res_svg_image : public image
{
public:
	const utki::shared_ref<ruis::render::renderer> renderer;

private:
	std::unique_ptr<svgdom::svg_element> dom;

public:
	res_svg_image( //
		utki::shared_ref<ruis::render::renderer> renderer,
		decltype(dom) dom
	) :
		renderer(std::move(renderer)),
		dom(std::move(dom))
	{}

	r4::vector2<uint32_t> dims(const ruis::units& units) const noexcept override
	{
		ASSERT(units.dots_per_pp() > 0)
		auto wh = this->dom->get_dimensions(units.dots_per_inch() / units.dots_per_pp());
		wh *= units.dots_per_pp();
		using std::ceil;
		return ceil(wh).to<uint32_t>();
	}

	utki::shared_ref<const render::texture_2d> get(
		const ruis::units& units, //
		vector2 for_dims
	) const override
	{
		// TODO: develop algorithm to go through cache from time to time and drop zombie textures
		if (auto i = this->cache.find(for_dims.to<unsigned>()); i != this->cache.end()) {
			if (auto p = i->second.lock()) {
				ASSERT(p)
				return utki::shared_ref(std::move(p));
			} else {
				this->cache.erase(i);
			}
		}

		utki::assert(this->dom, SL);

		// in ruis, SVG dimensions are in pp, this is why we cannot use 0 to use native dimension of SVG.
		// TODO: why can't we use 0?
		auto svg_dims = this->dims(units).to<real>();
		for (unsigned i = 0; i != 2; ++i) {
			if (for_dims[i] == 0) {
				for_dims[i] = svg_dims[i];
			}
		}

		svgren::parameters svg_params;
		svg_params.dpi = unsigned(units.dots_per_inch());
		svg_params.dims_request = for_dims.to<unsigned>();

		auto im = svgren::rasterize(
			*this->dom, //
			svg_params
		);

		utki::assert(im.dims().x() != 0, SL);
		utki::assert(im.dims().y() != 0, SL);
		utki::assert(
			size_t(im.dims().x()) * size_t(im.dims().y()) == im.pixels().size(), //
			[&](auto& o) {
				o << "im.dims = " << im.dims() << " pixels.size() = " << im.pixels().size();
			},
			SL
		);

		auto dims = im.dims();

		// clang-format off
		auto tex_2d = this->renderer.get().render_context.get().make_texture_2d(
			std::move(im),
			{
				.min_filter = render::texture_2d::filter::nearest,
				.mag_filter = render::texture_2d::filter::nearest,
				.mipmap = render::texture_2d::mipmap::none
			}
		);
		// clang-format on

		this->cache[dims] = tex_2d.to_shared_ptr();

		return tex_2d;
	}

	mutable std::map<
		r4::vector2<unsigned>, //
		std::weak_ptr<render::texture_2d> //
		>
		cache;

	static utki::shared_ref<res_svg_image> load( //
		const ruis::resource_loader& loader,
		const papki::file& fi
	)
	{
		auto dom = svgdom::load(fi);
		ASSERT(dom)
		return utki::make_shared<res_svg_image>(
			loader.renderer, //
			std::move(dom)
		);
	}
};
} // namespace

utki::shared_ref<image> image::load( //
	const ruis::resource_loader& loader,
	const tml::forest& desc,
	const papki::file& fi
)
{
	for (auto& p : desc) {
		if (p.value == "file") {
			fi.set_path(get_property_value(p).string);
			return image::load(
				loader, //
				fi
			);
		}
	}

	throw std::invalid_argument("image::load(): no 'file' property found in resource description");
}

utki::shared_ref<image> image::load( //
	const ruis::resource_loader& loader,
	const papki::file& fi
)
{
	if (fi.suffix().compare("svg") == 0) {
		return res_svg_image::load(loader, fi);
	} else {
		return res_raster_image::load(loader, fi);
	}
}
