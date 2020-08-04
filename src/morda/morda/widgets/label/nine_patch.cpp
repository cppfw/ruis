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
	this->img_matrix[0][0] = this->try_get_widget_as<image>("morda_lt");
	this->img_matrix[0][1] = this->try_get_widget_as<image>("morda_t");
	this->img_matrix[0][2] = this->try_get_widget_as<image>("morda_rt");

	this->img_matrix[1][0] = this->try_get_widget_as<image>("morda_l");
	this->img_matrix[1][1] = this->try_get_widget_as<image>("morda_m");
	this->img_matrix[1][2] = this->try_get_widget_as<image>("morda_r");

	this->img_matrix[2][0] = this->try_get_widget_as<image>("morda_lb");
	this->img_matrix[2][1] = this->try_get_widget_as<image>("morda_b");
	this->img_matrix[2][2] = this->try_get_widget_as<image>("morda_rb");

	this->on_blending_change();

	this->inner_content = this->try_get_widget_as<pile>("morda_content");

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
			this->set_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(*i).to_string()));
		}
	}

	this->inner_content->push_back_inflate(desc);
}

void nine_patch::render(const morda::matrix4& matrix)const{
	this->column::render(matrix);
}

void nine_patch::set_nine_patch(std::shared_ptr<const res::nine_patch> np){
	this->np_res = std::move(np);
	this->texture.reset();

	this->applyImages();

	this->clear_cache();
}

sides<real> nine_patch::get_actual_borders()const noexcept{
	sides<real> ret;

	for(auto i = 0; i != ret.size(); ++i){
		if(this->borders[i] >= 0){
			ret[i] = this->borders[i];
		}else if(!this->np_res){
			ret[i] = 0;
		}else{
			ret[i] = this->np_res->borders()[i];
		}
	}

	return ret;
}

void nine_patch::applyImages(){
	if(!this->np_res){
		for(auto& i : this->img_matrix){
			for(auto& j : i){
				j->set_image(nullptr);
			}
		}
		return;
	}

	auto& minBorders = this->np_res->borders();
//		TRACE(<< "minBorders = " << minBorders << std::endl)

	{
		// non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = this->img_matrix[0][0]->get_layout_params_const();

		if(this->borders.left() == layout_params::min){
			if(layoutParams.dims.x != minBorders.left()){
				auto& lp = this->img_matrix[0][0]->get_layout_params();
				lp.dims.x = minBorders.left();
			}
		}else{
			if(layoutParams.dims.x != this->borders.left()){
				auto& lp = this->img_matrix[0][0]->get_layout_params();
				lp.dims.x = this->borders.left();
			}
		}

		if(this->borders.top() == layout_params::min){
			if(layoutParams.dims.y != minBorders.top()){
				auto& lp = this->img_matrix[0][0]->get_layout_params();
				lp.dims.y = minBorders.top();
			}
		}else{
			if(layoutParams.dims.y != this->borders.top()){
				auto& lp = this->img_matrix[0][0]->get_layout_params();
				lp.dims.y = this->borders.top();
			}
		}
//			TRACE(<< "layoutParams.dim = " << layoutParams.dim << std::endl)
	}
	{
		// non-const call to get_layout_params requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = this->img_matrix[2][2]->get_layout_params_const();

		if(this->borders.right() == layout_params::min){
			if(layoutParams.dims.x != minBorders.right()){
				auto& lp = this->img_matrix[2][2]->get_layout_params();
				lp.dims.x = minBorders.right();
			}
		}else{
			if(layoutParams.dims.x != this->borders.right()){
				auto& lp = this->img_matrix[2][2]->get_layout_params();
				lp.dims.x = this->borders.right();
			}
		}

		if(this->borders.bottom() == layout_params::min){
			if(layoutParams.dims.y != minBorders.bottom()){
				auto& lp = this->img_matrix[2][2]->get_layout_params();
				lp.dims.y = minBorders.bottom();
			}
		}else{
			if(layoutParams.dims.y != this->borders.bottom()){
				auto& lp = this->img_matrix[2][2]->get_layout_params();
				lp.dims.y = this->borders.bottom();
			}
		}
//			TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
//		TRACE(<< "this->borders = " << this->borders << std::endl)

	this->texture = this->np_res->get(this->borders);

	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->img_matrix[i][j]->set_image(this->texture->images()[i][j]);
		}
	}
}

void nine_patch::set_center_visible(bool visible){
	ASSERT(this->img_matrix[1][1])
	this->img_matrix[1][1]->set_visible(visible);
}

void nine_patch::on_blending_change(){
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->img_matrix[i][j]->set_blending_params(this->get_blending_params());
		}
	}
}
