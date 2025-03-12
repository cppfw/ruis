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
class fixed_texture : public image::texture
{
protected:
	const utki::shared_ref<const render::texture_2d> tex_2d;

	fixed_texture(utki::shared_ref<const ruis::render::renderer> r, utki::shared_ref<const render::texture_2d> tex) :
		image::texture(std::move(r), tex.get().dims()),
		tex_2d(std::move(tex))
	{}

public:
	void render(const matrix4& matrix, const render::vertex_array& vao) const override
	{
		this->renderer.get().shaders().pos_tex->render(matrix, vao, this->tex_2d.get());
	}
};

class res_raster_image :
	public image, //
	public fixed_texture
{
public:
	res_raster_image( //
		utki::shared_ref<ruis::context> c,
		utki::shared_ref<const render::texture_2d> tex
	) :
		fixed_texture( //
			c.get().renderer,
			std::move(tex)
		)
	{}

	utki::shared_ref<const image::texture> get(
		const ruis::units& units, //
		vector2 for_dims
	) const override
	{
		return utki::make_shared_from(*this);
	}

	r4::vector2<uint32_t> dims(const ruis::units& units) const noexcept override
	{
		return this->tex_2d.get().dims();
	}

	static utki::shared_ref<res_raster_image> load( //
		utki::shared_ref<ruis::context> ctx,
		const papki::file& fi
	)
	{
		return utki::make_shared<res_raster_image>(
			std::move(ctx),
			ctx.get().renderer.get().render_context.get().create_texture_2d(
				rasterimage::read(fi),
				{
					// TODO: what about params?
				}
			)
		);
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

	class svg_texture : public fixed_texture
	{
		std::weak_ptr<const res_svg_image> parent;

	public:
		svg_texture(
			utki::shared_ref<const ruis::render::renderer> r,
			utki::shared_ref<const res_svg_image> parent,
			utki::shared_ref<const render::texture_2d> tex
		) :
			fixed_texture(std::move(r), std::move(tex)),
			parent(parent.to_shared_ptr())
		{}

		svg_texture(const svg_texture&) = delete;
		svg_texture& operator=(const svg_texture&) = delete;

		svg_texture(svg_texture&&) = delete;
		svg_texture& operator=(svg_texture&&) = delete;

		// NOLINTNEXTLINE(bugprone-exception-escape, "false positive")
		~svg_texture() override
		{
			if (auto p = this->parent.lock()) {
				r4::vector2<unsigned> d = this->tex_2d.get().dims().to<unsigned>();
				p->cache.erase(d);
			}
		}
	};

	utki::shared_ref<const texture> get(
		const ruis::units& units, //
		vector2 for_dims
	) const override
	{
		//		TRACE(<< "forDim = " << forDim << std::endl)

		{ // check if in cache
			auto i = this->cache.find(for_dims.to<unsigned>());
			if (i != this->cache.end()) {
				if (auto p = i->second.lock()) {
					ASSERT(p)
					return utki::shared_ref(std::move(p));
				}
			}
		}
		//		TRACE(<< "not in cache" << std::endl)

		ASSERT(this->dom)

		// in ruis, SVG dimensions are in pp, this is why we cannot use 0 to use native dimension of SVG.
		auto svg_dims = this->dims(units).to<real>();
		for (unsigned i = 0; i != 2; ++i) {
			if (for_dims[i] == 0) {
				for_dims[i] = svg_dims[i];
			}
		}

		svgren::parameters svg_params;
		svg_params.dpi = unsigned(units.dots_per_inch());
		svg_params.dims_request = for_dims.to<unsigned>();

		auto im = svgren::rasterize(*this->dom, svg_params);

		ASSERT(im.dims().x() != 0)
		ASSERT(im.dims().y() != 0)
		ASSERT(im.dims().x() * im.dims().y() == im.pixels().size(), [&](auto& o) {
			o << "im.dims = " << im.dims() << " pixels.size() = " << im.pixels().size();
		})

		auto dims = im.dims();

		// clang-format off
		auto img = utki::make_shared<svg_texture>(
			this->renderer,
			utki::make_shared_from(*this),
			this->renderer.get().render_context.get().create_texture_2d(
				std::move(im),
				{
					.min_filter = render::texture_2d::filter::nearest,
					.mag_filter = render::texture_2d::filter::nearest,
					.mipmap = render::texture_2d::mipmap::none
				}
			)
		);
		// clang-format on

		this->cache[dims] = img.to_shared_ptr();

		return img;
	}

	mutable std::map<r4::vector2<unsigned>, std::weak_ptr<texture>> cache;

	static utki::shared_ref<res_svg_image> load( //
		utki::shared_ref<ruis::context> ctx,
		const papki::file& fi
	)
	{
		auto dom = svgdom::load(fi);
		ASSERT(dom)
		return utki::make_shared<res_svg_image>(
			ctx.get().renderer, //
			std::move(dom)
		);
	}
};
} // namespace

utki::shared_ref<image> image::load( //
	utki::shared_ref<ruis::context> ctx,
	const tml::forest& desc,
	const papki::file& fi
)
{
	for (auto& p : desc) {
		if (p.value == "file") {
			fi.set_path(get_property_value(p).string);
			return image::load(std::move(ctx), fi);
		}
	}

	throw std::invalid_argument("image::load(): no 'file' property found in resource description");
}

utki::shared_ref<image> image::load( //
	utki::shared_ref<ruis::context> ctx,
	const papki::file& fi
)
{
	if (fi.suffix().compare("svg") == 0) {
		return res_svg_image::load(std::move(ctx), fi);
	} else {
		return res_raster_image::load(std::move(ctx), fi);
	}
}
