#include <utki/util.hpp>
#include <utki/types.hpp>

#include "../../context.hpp"
#include "../../util/util.hpp"

#include "nine_patch.hpp"

using namespace morda;

namespace{
const auto ninePatchLayout_c = puu::read(R"qwertyuiop(
		@row{
			layout{dx{fill}}
			@image{
				id{morda_lt}
			}

			@image{
				layout{dx{0}weight{1}}
				id{morda_t}
			}

			@image{
				id{morda_rt}
			}
		}

		@row{
			layout{
				dx{max}
				weight{1}
			}

			@image{
				id{morda_l}
				layout{dy{fill}}
			}

			@pile{
				id{morda_content}
				layout{
					weight{1}
					dy{max}
				}

				@image{
					id{morda_m}
					layout{dx{fill}dy{fill}}
				}
			}
			@image{
				id{morda_r}
				layout{dy{fill}}
			}
		}

		@row{
			layout{dx{fill}}
			@image{
				id{morda_lb}
			}

			@image{
				layout{dx{0}weight{1}}
				id{morda_b}
			}

			@image{
				id{morda_rb}
			}
		}
	)qwertyuiop");
}

nine_patch::nine_patch(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		blending_widget(this->context, desc),
		column(this->context, ninePatchLayout_c)
{
	this->imageMatrix_v[0][0] = this->try_get_widget_as<image>("morda_lt");
	this->imageMatrix_v[0][1] = this->try_get_widget_as<image>("morda_t");
	this->imageMatrix_v[0][2] = this->try_get_widget_as<image>("morda_rt");

	this->imageMatrix_v[1][0] = this->try_get_widget_as<image>("morda_l");
	this->imageMatrix_v[1][1] = this->try_get_widget_as<image>("morda_m");
	this->imageMatrix_v[1][2] = this->try_get_widget_as<image>("morda_r");

	this->imageMatrix_v[2][0] = this->try_get_widget_as<image>("morda_lb");
	this->imageMatrix_v[2][1] = this->try_get_widget_as<image>("morda_b");
	this->imageMatrix_v[2][2] = this->try_get_widget_as<image>("morda_rb");

	this->on_blending_changed();

	this->content_v = this->try_get_widget_as<pile>("morda_content");

	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "left"){
			// 'min' is by default, but not allowed to specify explicitly, as well as 'max' and 'fill',
			// so, use parse_dimension_value().
			this->borders.left() = parse_dimension_value(get_property_value(p), this->context->units);
		}else if(p.value == "right"){
			this->borders.right() = parse_dimension_value(get_property_value(p), this->context->units);
		}else if(p.value == "top"){
			this->borders.top() = parse_dimension_value(get_property_value(p), this->context->units);
		}else if(p.value == "bottom"){
			this->borders.bottom() = parse_dimension_value(get_property_value(p), this->context->units);
		}else if(p.value == "center_visible"){
			this->set_center_visible(get_property_value(p).to_bool());
		}
	}

	// this should go after setting up border widgets
	{
		auto i = std::find(desc.begin(), desc.end(), "image");
		if(i != desc.end()){
			this->set_nine_patch(this->context->loader.load<res_nine_patch>(get_property_value(*i).to_string()));
		}
	}

	this->content_v->inflate_push_back(desc);
}

void nine_patch::render(const morda::matrix4& matrix)const{
	this->column::render(matrix);
}

void nine_patch::set_nine_patch(std::shared_ptr<const res_nine_patch> np){
	this->res = std::move(np);
	this->scaledImage.reset();

	this->applyImages();

	this->clear_cache();
}

Sidesr nine_patch::get_actual_borders()const noexcept{
	Sidesr ret;

	for(auto i = 0; i != ret.size(); ++i){
		if(this->borders[i] >= 0){
			ret[i] = this->borders[i];
		}else if(!this->res){
			ret[i] = 0;
		}else{
			ret[i] = this->res->borders()[i];
		}
	}

	return ret;
}

void nine_patch::applyImages(){
	if(!this->res){
		for(auto& i : this->imageMatrix_v){
			for(auto& j : i){
				j->set_image(nullptr);
			}
		}
		return;
	}

	auto& minBorders = this->res->borders();
//		TRACE(<< "minBorders = " << minBorders << std::endl)

	{
		//non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = utki::makePtrToConst(this->imageMatrix_v[0][0].get())->get_layout_params();

		if(this->borders.left() == layout_params::min){
			if(layoutParams.dims.x != minBorders.left()){
				auto& lp = this->imageMatrix_v[0][0].get()->get_layout_params();
				lp.dims.x = minBorders.left();
			}
		}else{
			if(layoutParams.dims.x != this->borders.left()){
				auto& lp = this->imageMatrix_v[0][0].get()->get_layout_params();
				lp.dims.x = this->borders.left();
			}
		}

		if(this->borders.top() == layout_params::min){
			if(layoutParams.dims.y != minBorders.top()){
				auto& lp = this->imageMatrix_v[0][0].get()->get_layout_params();
				lp.dims.y = minBorders.top();
			}
		}else{
			if(layoutParams.dims.y != this->borders.top()){
				auto& lp = this->imageMatrix_v[0][0].get()->get_layout_params();
				lp.dims.y = this->borders.top();
			}
		}
//			TRACE(<< "layoutParams.dim = " << layoutParams.dim << std::endl)
	}
	{
		//non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = utki::makePtrToConst(this->imageMatrix_v[2][2].get())->get_layout_params();

		if(this->borders.right() == layout_params::min){
			if(layoutParams.dims.x != minBorders.right()){
				auto& lp = this->imageMatrix_v[2][2]->get_layout_params();
				lp.dims.x = minBorders.right();
			}
		}else{
			if(layoutParams.dims.x != this->borders.right()){
				auto& lp = this->imageMatrix_v[2][2]->get_layout_params();
				lp.dims.x = this->borders.right();
			}
		}

		if(this->borders.bottom() == layout_params::min){
			if(layoutParams.dims.y != minBorders.bottom()){
				auto& lp = this->imageMatrix_v[2][2]->get_layout_params();
				lp.dims.y = minBorders.bottom();
			}
		}else{
			if(layoutParams.dims.y != this->borders.bottom()){
				auto& lp = this->imageMatrix_v[2][2]->get_layout_params();
				lp.dims.y = this->borders.bottom();
			}
		}
//			TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
//		TRACE(<< "this->borders = " << this->borders << std::endl)

	this->scaledImage = this->res->get(this->borders);

	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageMatrix_v[i][j]->set_image(this->scaledImage->images()[i][j]);
		}
	}
}

void nine_patch::set_center_visible(bool visible){
	ASSERT(this->imageMatrix_v[1][1])
	this->imageMatrix_v[1][1]->set_visible(visible);
}

void nine_patch::on_blending_changed(){
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageMatrix_v[i][j]->set_blending_params(this->get_blending_params());
		}
	}
}
