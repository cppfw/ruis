#include "image.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

image::image(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		blending_widget(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "image"){
			this->img = this->context->loader.load<res::image>(get_property_value(p).to_string());
		}else if(p.value == "disabled_image"){
			this->disabled_img = this->context->loader.load<res::image>(get_property_value(p).to_string());
		}else if(p.value == "keep_aspect_ratio"){
			this->keep_aspect_ratio = get_property_value(p).to_bool();
		}else if(p.value == "repeat_x"){
			this->repeat_v.x = get_property_value(p).to_bool();
		}else if(p.value == "repeat_y"){
			this->repeat_v.y = get_property_value(p).to_bool();
		}
	}
}

namespace{
const std::array<r4::vec2f, 4> quadFanTexCoords = {{
	r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1)
}};
}

void image::render(const morda::matrix4& matrix) const{
	auto img = this->img.get();

	if(!this->is_enabled() && this->disabled_img){
		img = this->disabled_img.get();
	}

	if(!img){
		return;
	}

	this->set_blending_to_renderer();
	
	auto& r = *this->context->renderer;
	
	if(!this->texture){
		this->texture = img->get(this->rect().d);

		if(this->repeat_v.x || this->repeat_v.y){
			std::array<r4::vec2f, 4> texCoords;
			ASSERT(quadFanTexCoords.size() == texCoords.size())
			auto src = quadFanTexCoords.cbegin();
			auto dst = texCoords.begin();
			auto scale = this->rect().d.comp_divided(img->dims());
			if(!this->repeat_v.x){
				scale.x = 1;
			}
			if(!this->repeat_v.y){
				scale.y = 1;
			}
			for(; dst != texCoords.end(); ++src, ++dst){
				*dst = src->comp_multiplied(scale);
			}
			this->vao = r.factory->create_vertex_array(
					{
						r.quad_01_vbo,
						r.factory->create_vertex_buffer(utki::make_span(texCoords))
					},
					r.quad_indices,
					vertex_array::mode::triangle_fan
				);
		}else{
			this->vao = this->context->renderer->pos_tex_quad_01_vao;
		}
	}
	ASSERT(this->texture)

	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	this->texture->render(matr, *this->vao);
}

morda::vector2 image::measure(const morda::vector2& quotum)const{
	auto img = this->img.get();

	if(!this->is_enabled() && this->disabled_img){
		img = this->disabled_img.get();
	}

	if(!img){
		return vector2(0);
	}
	
	vector2 imgDim = img->dims(this->context->units.dots_per_inch);
	
	ASSERT_INFO(imgDim.is_positive_or_zero(), "imgDim = " << imgDim)
	
	if(!this->keep_aspect_ratio){
		vector2 ret = imgDim;
		
		for(unsigned i = 0; i != ret.size(); ++i){
			if(quotum[i] >= 0){
				ret[i] = quotum[i];
			}
		}

		return ret;
	}
	
	ASSERT(img)
	ASSERT(imgDim.y > 0)
	
	real ratio = imgDim.x / imgDim.y;
	
	if(quotum.x < 0 && quotum.y < 0){
		return imgDim;
	}else if(quotum.x < 0){
		ASSERT(quotum.y >= 0)
		
		vector2 ret;
		ret.y = quotum.y;
		ret.x = ratio * quotum.y;
		return ret;
	}else if(quotum.y >= 0){
		ASSERT(quotum.x >= 0)
		ASSERT(quotum.y >= 0)
		// This case is possible when image layout parameters are, for example 'dx{max} dy{fill}', so the
		// minimum x size will be determined to keep aspect ratio, but later, the x size of the image widget can be
		// set to fill all the allowed space, in this case the measure() method will be called with
		// both quotum components to be positive numbers.
		return quotum;
	}else{
		ASSERT(quotum.x >= 0)
		ASSERT_INFO(quotum.y < 0, "quotum =" << quotum)
		
		ASSERT(ratio > 0)
		
		vector2 ret;
		ret.x = quotum.x;
		ret.y = quotum.x / ratio;
		return ret;
	}
}

void image::set_image(std::shared_ptr<const res::image> image){
	if(this->img && image && this->img->dims() == image->dims()){
	}else{
		this->invalidate_layout();
	}
	
	this->img = std::move(image);
	this->texture.reset();
}

void image::set_disabled_image(std::shared_ptr<const res::image> image){
	if(this->disabled_img && image && this->disabled_img->dims() == image->dims()){
	}else{
		if(!this->is_enabled()){
			this->invalidate_layout();
		}
	}
	
	this->disabled_img = std::move(image);
	this->texture.reset();
}

void image::on_resize(){
	this->widget::on_resize();
	this->texture.reset();
}

void image::on_enable_change(){
	if(this->disabled_img && this->img){
		// if dimension of active image change then need to re-layout
		if(this->disabled_img->dims() != this->img->dims()){
			this->invalidate_layout();
		}
	}else{
		// if there is only disabled image, perhaps nobody will ever use it like this, but nevertheless,
		// let's handle this case gracefully
		if(this->disabled_img){
			this->invalidate_layout();
		}
	}
}
