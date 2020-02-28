#include "res_nine_patch.hpp"

#include <iomanip>

#include "../context.hpp"

#include "../util/util.hpp"

using namespace morda;

namespace{

class ResSubImage :
		public res_image,
		public res_image::texture
{
	friend class res_image;
	
	std::shared_ptr<const res_image::texture> tex;
	
	std::shared_ptr<vertex_array> vao;
	
public:
	// rect is a rectangle on the texture, Y axis down.
	ResSubImage(std::shared_ptr<morda::context> c, decltype(tex) tex, const rectangle& rect) :
			res_image(c),
			res_image::texture(c->renderer, rect.d),
			tex(std::move(tex))
	{
		std::array<vector2, 4> texCoords;
		
		texCoords[0] = rect.p.compDiv(this->tex->dims);
		texCoords[1] = rect.leftTop().compDiv(this->tex->dims);
		texCoords[2] = rect.rightTop().compDiv(this->tex->dims);
		texCoords[3] = rect.rightBottom().compDiv(this->tex->dims);
//		TRACE(<< "this->texCoords = (" << texCoords[0] << ", " << texCoords[1] << ", " << texCoords[2] << ", " << texCoords[3] << ")" << std::endl)
		auto& r = *this->context->renderer;
		this->vao = r.factory->create_vertex_array(
				{
					r.quad_01_vbo,
					r.factory->create_vertex_buffer(utki::make_span(texCoords))
				},
				r.quad_indices,
				vertex_array::mode::triangle_fan
			);
	}
	
	ResSubImage(const ResSubImage& orig) = delete;
	ResSubImage& operator=(const ResSubImage& orig) = delete;
	
	vector2 dims(real dpi) const noexcept override{
		return this->res_image::texture::dims;
	}
	
	virtual std::shared_ptr<const res_image::texture> get(vector2 forDim)const override{
		return this->sharedFromThis(this);
	}
	
	void render(const matrix4& matrix, const vertex_array& vao) const override{
		ASSERT(this->tex)
		this->tex->render(matrix, *this->vao);
	}
};

}

std::shared_ptr<res_nine_patch> res_nine_patch::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	sides<real> borders(-1);
	for(auto& p : desc){
		if(p.value == "borders"){
			borders = parse_sides(p.children);
		}else if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}
	if(borders.left() < 0){
		throw std::runtime_error("res_nine_patch::load(): could not read borders");
	}

	auto image = res_image::load(ctx, fi);
	
	return std::make_shared<res_nine_patch>(ctx.shared_from_this(), image, borders);
}

res_nine_patch::image_matrix::image_matrix(std::array<std::array<std::shared_ptr<const res_image>, 3>, 3>&& l, std::shared_ptr<const res_nine_patch> parent, real mul) :
		images_v(l),
		parent(parent),
		mul(mul)
{}

res_nine_patch::image_matrix::~image_matrix()noexcept{
	if(auto p = this->parent.lock()){		
		p->cache.erase(this->mul);
	}
}

std::shared_ptr<res_nine_patch::image_matrix> res_nine_patch::get(sides<real> borders) const {
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
	
	auto quadTex = this->image->get((this->image->dims() * mul).round());
//	TRACE(<< "quadTex->dim() = " << quadTex->dim() << std::endl)
	
	vector2 actMul = quadTex->dims.compDiv(this->image->dims());

	
//	TRACE(<< "actMul = " << std::setprecision(10) << actMul << std::endl)
	
	sides<real> scaledBorders(this->borders_v);
	scaledBorders.left() *= actMul.x;
	scaledBorders.right() *= actMul.x;
	scaledBorders.top() *= actMul.y;
	scaledBorders.bottom() *= actMul.y;
	
//	TRACE(<< "scaledBorders = " << std::setprecision(10) << scaledBorders << std::endl)
	
	auto ret = std::make_shared<image_matrix>(
			std::array<std::array<std::shared_ptr<const res_image>, 3>, 3>({{
				{{
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							0,
							0,
							scaledBorders.left(),
							scaledBorders.top())
						), // left top
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							scaledBorders.left(),
							0,
							std::round(quadTex->dims.x - scaledBorders.left() - scaledBorders.right()),
							scaledBorders.top())
						), // top
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							std::round(quadTex->dims.x - scaledBorders.right()),
							0,
							scaledBorders.right(),
							scaledBorders.top())
						) // right top
				}},
				{{
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							0,
							scaledBorders.top(),
							scaledBorders.left(),
							std::round(quadTex->dims.y - scaledBorders.top() - scaledBorders.bottom()))
						), // left
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							scaledBorders.left(),
							scaledBorders.top(),
							std::round(quadTex->dims.x - scaledBorders.left() - scaledBorders.right()),
							std::round(quadTex->dims.y - scaledBorders.top() - scaledBorders.bottom()))
						), // middle
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							std::round(quadTex->dims.x - scaledBorders.right()),
							scaledBorders.top(),
							scaledBorders.right(),
							std::round(quadTex->dims.y - scaledBorders.top() - scaledBorders.bottom()))
						) // right
				}},
				{{
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							0,
							std::round(quadTex->dims.y - scaledBorders.bottom()),
							scaledBorders.left(),
							scaledBorders.bottom())
						), // left bottom
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							scaledBorders.left(),
							std::round(quadTex->dims.y - scaledBorders.bottom()),
							std::round(quadTex->dims.x - scaledBorders.left() - scaledBorders.right()),
							scaledBorders.bottom())
						), // bottom
					std::make_shared<ResSubImage>(this->context, quadTex, rectangle(
							std::round(quadTex->dims.x - scaledBorders.right()),
							std::round(quadTex->dims.y - scaledBorders.bottom()),
							scaledBorders.right(),
							scaledBorders.bottom())
						) // right bottom
				}}
			}}),
			this->sharedFromThis(this),
			mul
		);
	
	this->cache[mul] = ret;
	return ret;
}
