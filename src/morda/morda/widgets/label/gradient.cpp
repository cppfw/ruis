#include "gradient.hpp"

#include "../../util/util.hpp"

#include "../../context.hpp"

using namespace morda;

gradient::gradient(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "gradient"){
			this->res = this->context->loader.load<res::gradient>(get_property_value(p).to_string());
		}
	}
}

void gradient::render(const matrix4& matrix)const{
	set_simple_alpha_blending(*this->context->renderer);
	
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	if(this->res){
//		TRACE(<< "this->rect().d = " << this->rect().d << std::endl)
		this->res->render(matr);
	}
}
