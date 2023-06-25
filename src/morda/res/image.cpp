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

using namespace morda;
using namespace morda::res;

image::image(const utki::shared_ref<morda::context>& c) :
	resource(c)
{}

// atlas_image::atlas_image(
// 	const utki::shared_ref<morda::context>& c,
// 	const utki::shared_ref<res::texture>& tex,
// 	const rectangle& rect
// ) :
// 		image(c),
// 		image::texture(this->context.get().renderer, abs(rect.d)),
// 		tex(tex)
// {
// //	this->texCoords[3] = vector2(rect.left(), this->tex->tex().dim().y -
// rect.bottom()).compDivBy(this->tex->tex().dim());
// //	this->texCoords[2] = vector2(rect.right(), this->tex->tex().dim().y -
// rect.bottom()).compDivBy(this->tex->tex().dim());
// //	this->texCoords[1] = vector2(rect.right(), this->tex->tex().dim().y -
// rect.top()).compDivBy(this->tex->tex().dim());
// //	this->texCoords[0] = vector2(rect.left(), this->tex->tex().dim().y -
// rect.top()).compDivBy(this->tex->tex().dim());
// 	//TODO:
// 	ASSERT(false)
// }

atlas_image::atlas_image(const utki::shared_ref<morda::context>& c, const utki::shared_ref<res::texture>& tex) :
	image(c),
	image::texture(this->context.get().renderer, tex.get().tex().dims()),
	tex(std::move(tex)),
	vao(this->context.get().renderer.get().pos_tex_quad_01_vao)
{}

utki::shared_ref<atlas_image> atlas_image::load(
	const utki::shared_ref<morda::context>& ctx,
	const treeml::forest& desc,
	const papki::file& fi
)
{
	std::shared_ptr<res::texture> tex;
	rectangle rect(-1, -1);

	for (auto& p : desc) {
		if (p.value == "tex") {
			tex = ctx.get()
					  .loader.load<res::texture>(get_property_value(p).to_string())
					  .to_shared_ptr(); // TODO: do not use to_shared_ptr() here
		} else if (p.value == "rect") {
			rect = parse_rect(p.children);
		}
	}

	if (!tex) {
		throw std::runtime_error("atlas_image::load(): could not load texture");
	}

	// TODO:
	// if(rect.p.x() >= 0){
	// 	return utki::make_shared<atlas_image>(ctx, utki::shared_ref(std::move(tex)), rect);
	// }else{
	return utki::make_shared<atlas_image>(ctx, utki::shared_ref(std::move(tex)));
	// }
}

void atlas_image::render(const matrix4& matrix, const vertex_array& vao) const
{
	this->context.get().renderer.get().shader->pos_tex->render(matrix, this->vao.get(), this->tex.get().tex());
}

utki::shared_ref<const image::texture> atlas_image::get(vector2 for_dims) const
{
	return utki::make_shared_from(*this);
}

namespace {
class fixed_texture : public image::texture
{
protected:
	const utki::shared_ref<const texture_2d> tex_2d;

	fixed_texture(const utki::shared_ref<const morda::renderer>& r, const utki::shared_ref<const texture_2d>& tex) :
		image::texture(r, tex.get().dims()),
		tex_2d(tex)
	{}

public:
	void render(const matrix4& matrix, const vertex_array& vao) const override
	{
		this->renderer.get().shader->pos_tex->render(matrix, vao, this->tex_2d.get());
	}
};

class res_raster_image : public image, public fixed_texture
{
public:
	res_raster_image(const utki::shared_ref<morda::context>& c, const utki::shared_ref<const texture_2d>& tex) :
		image(c),
		fixed_texture(this->context.get().renderer, tex)
	{}

	utki::shared_ref<const image::texture> get(vector2 for_dims) const override
	{
		return utki::make_shared_from(*this);
	}

	vector2 dims(real dpi) const noexcept override
	{
		return this->tex_2d.get().dims();
	}

	static utki::shared_ref<res_raster_image> load(const utki::shared_ref<morda::context>& ctx, const papki::file& fi)
	{
		return utki::make_shared<res_raster_image>(ctx, load_texture(ctx.get().renderer.get(), fi));
	}
};

class res_svg_image : public image
{
	std::unique_ptr<svgdom::svg_element> dom;

public:
	res_svg_image(const utki::shared_ref<morda::context>& c, decltype(dom) dom) :
		image(c),
		dom(std::move(dom))
	{}

	vector2 dims(real dpi) const noexcept override
	{
		auto wh = this->dom->get_dimensions(dpi);
		using std::ceil;
		return vector2(ceil(wh[0]), ceil(wh[1]));
	}

	class svg_texture : public fixed_texture
	{
		std::weak_ptr<const res_svg_image> parent;

	public:
		svg_texture(
			const utki::shared_ref<const morda::renderer>& r,
			const utki::shared_ref<const res_svg_image>& parent,
			const utki::shared_ref<const texture_2d>& tex
		) :
			fixed_texture(r, tex),
			parent(parent.to_shared_ptr())
		{}

		~svg_texture() override
		{
			if (auto p = this->parent.lock()) {
				r4::vector2<unsigned> d = this->tex_2d.get().dims().to<unsigned>();
				p->cache.erase(d);
			}
		}
	};

	utki::shared_ref<const texture> get(vector2 for_dims) const override
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
		//		TRACE(<< "width = " << this->dom->width << std::endl)
		//		TRACE(<< "height = " << this->dom->height << std::endl)
		//		TRACE(<< "dpi = " << morda::gui::inst().units.dpi() << std::endl)
		//		TRACE(<< "id = " << this->dom->id << std::endl)
		svgren::parameters svg_params;
		svg_params.dpi = unsigned(this->context.get().units.dots_per_inch);
		svg_params.dims_request = for_dims.to<unsigned>();

		auto im = svgren::rasterize(*this->dom, svg_params);
		ASSERT(im.dims().x() != 0)
		ASSERT(im.dims().y() != 0)
		ASSERT(im.dims().x() * im.dims().y() == im.pixels().size(), [&](auto& o) {
			o << "im.dims = " << im.dims() << " pixels.size() = " << im.pixels().size();
		})

		auto img = utki::make_shared<svg_texture>(
			this->context.get().renderer,
			utki::make_shared_from(*this),
			this->context.get().renderer.get().factory->create_texture_2d(std::move(im))
		);

		this->cache[im.dims()] = img.to_shared_ptr();

		return img;
	}

	mutable std::map<r4::vector2<unsigned>, std::weak_ptr<texture>> cache;

	static utki::shared_ref<res_svg_image> load(const utki::shared_ref<morda::context>& ctx, const papki::file& fi)
	{
		return utki::make_shared<res_svg_image>(ctx, svgdom::load(fi));
	}
};
} // namespace

utki::shared_ref<image> image::load(
	const utki::shared_ref<morda::context>& ctx,
	const treeml::forest& desc,
	const papki::file& fi
)
{
	for (auto& p : desc) {
		if (p.value == "file") {
			fi.set_path(get_property_value(p).to_string());
			return image::load(ctx, fi);
		}
	}

	return atlas_image::load(ctx, desc, fi);
}

utki::shared_ref<image> image::load(const utki::shared_ref<morda::context>& ctx, const papki::file& fi)
{
	if (fi.suffix().compare("svg") == 0) {
		return res_svg_image::load(ctx, fi);
	} else {
		return res_raster_image::load(ctx, fi);
	}
}
