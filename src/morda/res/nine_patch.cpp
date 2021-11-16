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

#include "nine_patch.hpp"

#include <iomanip>

#include "../context.hpp"
#include "../render/vertex_array.hpp"
#include "../util/util.hpp"

using namespace morda;
using namespace morda::res;

namespace{

class res_subimage :
		public res::image,
		public res::image::texture
{
	friend class res::image;
	
	std::shared_ptr<const res::image::texture> tex;
	
	std::shared_ptr<vertex_array> vao;
	
public:
	// rect is a rectangle on the texture, Y axis down.
	res_subimage(std::shared_ptr<morda::context> c, decltype(tex) tex, const rectangle& rect) :
			res::image(c),
			res::image::texture(c->renderer, rect.d),
			tex(std::move(tex))
	{
		std::array<vector2, 4> tex_coords = {
			rect.p.comp_div(this->tex->dims),
			rect.x1_y2().comp_div(this->tex->dims),
			rect.x2_y2().comp_div(this->tex->dims),
			rect.x2_y1().comp_div(this->tex->dims),
		};

		// TRACE(<< "this->tex_coords = (" << tex_coords[0] << ", " << tex_coords[1] << ", " << tex_coords[2] << ", " << tex_coords[3] << ")" << std::endl)

		auto& r = *this->context->renderer;
		this->vao = r.factory->create_vertex_array(
				{
					r.quad_01_vbo,
					r.factory->create_vertex_buffer(tex_coords)
				},
				r.quad_indices,
				vertex_array::mode::triangle_fan
			);
	}
	
	res_subimage(const res_subimage& orig) = delete;
	res_subimage& operator=(const res_subimage& orig) = delete;
	
	vector2 dims(real dpi)const noexcept override{
		return this->res::image::texture::dims;
	}
	
	virtual std::shared_ptr<const res::image::texture> get(vector2 for_dims)const override{
		return utki::make_shared_from(*this);
	}
	
	void render(const matrix4& matrix, const vertex_array& vao)const override{
		ASSERT(this->tex)
		this->tex->render(matrix, *this->vao);
	}
};

}

std::shared_ptr<nine_patch> nine_patch::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi){
	sides<real> borders(-1);
	for(auto& p : desc){
		if(p.value == "borders"){
			borders = parse_sides(p.children);
		}else if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}
	if(borders.left() < 0){
		throw std::runtime_error("nine_patch::load(): could not read borders");
	}

	auto image = res::image::load(ctx, fi);
	
	return std::make_shared<nine_patch>(utki::make_shared_from(ctx), image, borders);
}

nine_patch::image_matrix::image_matrix(
		std::array<std::array<std::shared_ptr<const res::image>, 3>, 3>&& l,
		std::weak_ptr<const nine_patch> parent,
		real mul
	) :
		img_matrix(l),
		parent(std::move(parent)),
		mul(mul)
{}

nine_patch::image_matrix::~image_matrix()noexcept{
	if(auto p = this->parent.lock()){		
		p->cache.erase(this->mul);
	}
}

std::shared_ptr<nine_patch::image_matrix> nine_patch::get(sides<real> borders)const{
	real mul = 1;
	{
		auto req = borders.begin();
		for(auto orig = this->borders_v.begin(); orig != this->borders_v.end(); ++orig, ++req){
			if(*orig <= 0 || *req <= 0){
				continue;
			}

			if(*req > (*orig) * mul){
				mul = *req / *orig;
			}
		}
	}
	
	{
		auto i = this->cache.find(mul);
		if(i != this->cache.end()){
			if(auto r = i->second.lock()){
				return r;
			}
		}
	}
	
//	TRACE(<< "this->image->dim() = " << std::setprecision(10) << this->image->dim() << std::endl)
//	TRACE(<< "mul = " << std::setprecision(10) << mul << std::endl)
	
	using std::round;
	auto quad_tex = this->image->get(round(this->image->dims() * mul));
//	TRACE(<< "quad_tex->dim() = " << quad_tex->dim() << std::endl)
	
	vector2 act_mul = quad_tex->dims.comp_div(this->image->dims());
	
//	TRACE(<< "act_mul = " << std::setprecision(10) << act_mul << std::endl)
	
	sides<real> scaled_borders(this->borders_v);
	scaled_borders.left() *= act_mul.x();
	scaled_borders.right() *= act_mul.x();
	scaled_borders.top() *= act_mul.y();
	scaled_borders.bottom() *= act_mul.y();
	
	// TRACE(<< "scaled_borders = " << std::setprecision(10) << scaled_borders << std::endl)
	
	auto ret = std::make_shared<image_matrix>(
			std::array<std::array<std::shared_ptr<const res::image>, 3>, 3>({{
				{{
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							0,
							0,
							scaled_borders.left(),
							scaled_borders.top())
						), // left top
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							scaled_borders.left(),
							0,
							std::round(quad_tex->dims.x() - scaled_borders.left() - scaled_borders.right()),
							scaled_borders.top())
						), // top
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							std::round(quad_tex->dims.x() - scaled_borders.right()),
							0,
							scaled_borders.right(),
							scaled_borders.top())
						) // right top
				}},
				{{
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							0,
							scaled_borders.top(),
							scaled_borders.left(),
							std::round(quad_tex->dims.y() - scaled_borders.top() - scaled_borders.bottom()))
						), // left
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							scaled_borders.left(),
							scaled_borders.top(),
							std::round(quad_tex->dims.x() - scaled_borders.left() - scaled_borders.right()),
							std::round(quad_tex->dims.y() - scaled_borders.top() - scaled_borders.bottom()))
						), // middle
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							std::round(quad_tex->dims.x() - scaled_borders.right()),
							scaled_borders.top(),
							scaled_borders.right(),
							std::round(quad_tex->dims.y() - scaled_borders.top() - scaled_borders.bottom()))
						) // right
				}},
				{{
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							0,
							std::round(quad_tex->dims.y() - scaled_borders.bottom()),
							scaled_borders.left(),
							scaled_borders.bottom())
						), // left bottom
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							scaled_borders.left(),
							std::round(quad_tex->dims.y() - scaled_borders.bottom()),
							std::round(quad_tex->dims.x() - scaled_borders.left() - scaled_borders.right()),
							scaled_borders.bottom())
						), // bottom
					std::make_shared<res_subimage>(this->context, quad_tex, rectangle(
							std::round(quad_tex->dims.x() - scaled_borders.right()),
							std::round(quad_tex->dims.y() - scaled_borders.bottom()),
							scaled_borders.right(),
							scaled_borders.bottom())
						) // right bottom
				}}
			}}),
			utki::make_shared_from(*this),
			mul
		);
	
	this->cache[mul] = ret;
	return ret;
}
