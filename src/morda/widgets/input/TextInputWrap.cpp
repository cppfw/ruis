#include "TextInputWrap.hpp"

#include "../../util/util.hpp"

using namespace morda;


TextInputWrap::TextInputWrap(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		text_widget(this->context, desc),
		List(this->context, puu::forest(), true)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "text"){
			this->set_text(unikod::toUtf32(get_property_value(p).to_string()));
		}
	}
}
