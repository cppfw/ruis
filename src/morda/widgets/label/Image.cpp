#include "Image.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"


using namespace morda;



Image::Image(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		BlendingWidget(c, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "image"){
			this->img = this->context->loader.load<ResImage>(get_property_value(p).to_string());
			this->resize(this->img->dims());
		}else if(p.value == "keepAspectRatio"){
			this->keepAspectRatio = get_property_value(p).to_bool();
		}else if(p.value == "repeatX"){
			this->repeat_v.x = get_property_value(p).to_bool();
		}else if(p.value == "repeatY"){
			this->repeat_v.y = get_property_value(p).to_bool();
		}
	}
}

namespace{
const std::array<r4::vec2f, 4> quadFanTexCoords = {{
	r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1)
}};
}

void Image::render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}

	this->applyBlending();
	
	auto& r = *this->context->renderer;
	
	if(!this->scaledImage){
		this->scaledImage = this->img->get(this->rect().d);

		if(this->repeat_v.x || this->repeat_v.y){
			std::array<r4::vec2f, 4> texCoords;
			ASSERT(quadFanTexCoords.size() == texCoords.size())
			auto src = quadFanTexCoords.cbegin();
			auto dst = texCoords.begin();
			auto scale = this->rect().d.compDiv(this->img->dims());
			if(!this->repeat_v.x){
				scale.x = 1;
			}
			if(!this->repeat_v.y){
				scale.y = 1;
			}
			for(; dst != texCoords.end(); ++src, ++dst){
				*dst = src->compMul(scale);
			}
			this->vao = r.factory->createVertexArray({r.quad01VBO, r.factory->createVertexBuffer(utki::wrapBuf(texCoords))}, r.quadIndices, VertexArray::Mode_e::TRIANGLE_FAN);
		}else{
			this->vao = this->context->renderer->posTexQuad01VAO;
		}
	}
	ASSERT(this->scaledImage)

	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	this->scaledImage->render(matr, *this->vao);
}

morda::Vec2r Image::measure(const morda::Vec2r& quotum)const{
	if(!this->img){
		return Vec2r(0);
	}
	
	Vec2r imgDim = this->img->dims(this->context->units.dots_per_inch);
	
	ASSERT_INFO(imgDim.isPositiveOrZero(), "imgDim = " << imgDim)
	
	if(!keepAspectRatio){
		Vec2r ret = imgDim;
		
		for(unsigned i = 0; i != ret.size(); ++i){
			if(quotum[i] >= 0){
				ret[i] = quotum[i];
			}
		}

		return ret;
	}
	
	ASSERT(this->img)
	ASSERT(imgDim.y > 0)
	
	real ratio = imgDim.x / imgDim.y;
	
	if(quotum.x < 0 && quotum.y < 0){
		return imgDim;
	}else if(quotum.x < 0){
		ASSERT(quotum.y >= 0)
		
		Vec2r ret;
		ret.y = quotum.y;
		ret.x = ratio * quotum.y;
		return ret;
	}else if(quotum.y >= 0){
		ASSERT(quotum.x >= 0)
		ASSERT(quotum.y >= 0)
		//This case is possible when Image layout parameters are, for example 'dx{max} dy{fill}', so the
		//minimum x size will be determined to keep aspect ratio, but later, the x size of the Image widget can be
		//set to fill all the allowed space, in this case the measure() method will be called with
		//both quotum components to be positive numbers.
		return quotum;
	}else{
		ASSERT(quotum.x >= 0)
		ASSERT_INFO(quotum.y < 0, "quotum =" << quotum)
		
		ASSERT(ratio > 0)
		
		Vec2r ret;
		ret.x = quotum.x;
		ret.y = quotum.x / ratio;
		return ret;
	}
}



void Image::setImage(const std::shared_ptr<const ResImage>& image) {
	if(this->img && image && this->img->dims() == image->dims()){
	}else{
		this->invalidate_layout();
	}
	
	this->img = image;
	this->scaledImage.reset();
}

void Image::on_resize() {
	this->Widget::on_resize();
	this->scaledImage.reset();
}
