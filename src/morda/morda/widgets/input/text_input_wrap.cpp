#include "text_input_wrap.hpp"

#include "../../util/util.hpp"

using namespace morda;

text_input_wrap::text_input_wrap(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		text_widget(this->context, desc),
		list_widget(this->context, puu::forest(), true)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "text"){
			this->set_text(utki::to_utf32(get_property_value(p).to_string()));
		}
	}
}
