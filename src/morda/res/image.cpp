/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "image.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

using namespace morda;
using namespace morda::res;

image::image(std::shared_ptr<morda::context> c) :
		resource(std::move(c))
{}

atlas_image::atlas_image(std::shared_ptr<morda::context> c, std::shared_ptr<res::texture> tex, const rectangle& rect) :
		image(std::move(c)),
		image::texture(this->context->renderer, abs(rect.d)),
		tex(std::move(tex))
{
//	this->texCoords[3] = vector2(rect.left(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[2] = vector2(rect.right(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[1] = vector2(rect.right(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
//	this->texCoords[0] = vector2(rect.left(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
	//TODO:
	ASSERT(false)
}

atlas_image::atlas_image(std::shared_ptr<morda::context> c, std::shared_ptr<res::texture> tex) :
		image(std::move(c)),
		image::texture(this->context->renderer, tex->tex().dims()),
		tex(std::move(tex)),
		vao(this->context->renderer->pos_tex_quad_01_vao)
{}

std::shared_ptr<atlas_image> atlas_image::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi){
	std::shared_ptr<res::texture> tex;
	rectangle rect(-1, -1);

	for(auto& p : desc){
		if(p.value == "tex"){
			tex = ctx.loader.load<res::texture>(get_property_value(p).to_string());
		}else if(p.value == "rect"){
			rect = parse_rect(p.children);
		}
	}

	if(!tex){
		throw std::runtime_error("atlas_image::load(): could not load texture");
	}
	
	if(rect.p.x() >= 0){
		return std::make_shared<atlas_image>(utki::make_shared_from(ctx), tex, rect);
	}else{
		return std::make_shared<atlas_image>(utki::make_shared_from(ctx), tex);
	}
}

void atlas_image::render(const matrix4& matrix, const vertex_array& vao)const{
	this->context->renderer->shader->pos_tex->render(matrix, *this->vao, this->tex->tex());
}

std::shared_ptr<const image::texture> atlas_image::get(vector2 forDim)const{
	return utki::make_shared_from(*this);
}

namespace{
class fixed_texture : public image::texture{
protected:
	std::shared_ptr<texture_2d> tex_v;
	
	fixed_texture(std::shared_ptr<morda::renderer> r, std::shared_ptr<texture_2d> tex) :
			image::texture(std::move(r), tex->dims()),
			tex_v(std::move(tex))
	{}
	
public:
	void render(const matrix4& matrix, const vertex_array& vao)const override{
		this->renderer->shader->pos_tex->render(matrix, vao, *this->tex_v);
	}
};
	
class res_raster_image :
		public image,
		public fixed_texture
{
public:
	res_raster_image(std::shared_ptr<morda::context> c, std::shared_ptr<texture_2d> tex) :
			image(std::move(c)),
			fixed_texture(this->context->renderer, std::move(tex))
	{}
	
	std::shared_ptr<const image::texture> get(vector2 forDim)const override{
		return utki::make_shared_from(*this);
	}
	
	vector2 dims(real dpi)const noexcept override{
		return this->tex_v->dims();
	}
	
	static std::shared_ptr<res_raster_image> load(morda::context& ctx, const papki::file& fi){
		return std::make_shared<res_raster_image>(utki::make_shared_from(ctx), load_texture(*ctx.renderer, fi));
	}
};

class res_svg_image : public image{
	std::unique_ptr<svgdom::svg_element> dom;
public:
	res_svg_image(std::shared_ptr<morda::context> c, decltype(dom) dom) :
			image(std::move(c)),
			dom(std::move(dom))
	{}
	
	vector2 dims(real dpi)const noexcept override{
		auto wh = this->dom->get_dimensions(dpi);
		using std::ceil;
		return vector2(ceil(wh[0]), ceil(wh[1]));
	}
	
	class svg_texture : public fixed_texture{
		std::weak_ptr<const res_svg_image> parent;
	public:
		svg_texture(std::shared_ptr<morda::renderer> r, std::shared_ptr<const res_svg_image> parent, std::shared_ptr<texture_2d> tex) :
				fixed_texture(std::move(r), std::move(tex)),
				parent(parent)
		{}

		~svg_texture()noexcept{
			if(auto p = this->parent.lock()){
				r4::vector2<unsigned> d = this->tex_v->dims().to<unsigned>();
				p->cache.erase(d);
			}
		}
	};
	
	std::shared_ptr<const texture> get(vector2 forDim)const override{
//		TRACE(<< "forDim = " << forDim << std::endl)

		{ // check if in cache
			auto i = this->cache.find(forDim.to<unsigned>());
			if(i != this->cache.end()){
				if(auto p = i->second.lock()){
					return p;
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
		svg_params.dpi = unsigned(this->context->units.dots_per_inch);
		svg_params.dims_request = forDim.to<unsigned>();
		auto svg = svgren::render(*this->dom, svg_params);
		ASSERT(svg.dims.x() != 0)
		ASSERT(svg.dims.y() != 0)
		ASSERT(
			svg.dims.x() * svg.dims.y() == svg.pixels.size(),
			[&](auto&o){o << "svg.dims = " << svg.dims << " pixels.size() = " << svg.pixels.size();}
		)
		
		auto img = std::make_shared<svg_texture>(
				this->context->renderer,
				utki::make_shared_from(*this),
				this->context->renderer->factory->create_texture_2d(svg.dims, utki::make_span(svg.pixels))
			);

		this->cache[svg.dims] = img;

		return img;
	}
	
	mutable std::map<r4::vector2<unsigned>, std::weak_ptr<texture>> cache;
	
	static std::shared_ptr<res_svg_image> load(morda::context& ctx, const papki::file& fi){
		return std::make_shared<res_svg_image>(utki::make_shared_from(ctx), svgdom::load(fi));
	}	
};
}

std::shared_ptr<image> image::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi) {
	for(auto& p : desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
			return image::load(ctx, fi);
		}
	}

	return atlas_image::load(ctx, desc, fi);
}

std::shared_ptr<image> image::load(morda::context& ctx, const papki::file& fi) {
	if(fi.suffix().compare("svg") == 0){
		return res_svg_image::load(ctx, fi);
	}else{
		return res_raster_image::load(ctx, fi);
	}
}
