#include "TextInputWrap.hpp"

#include "../../util/util.hpp"

using namespace morda;


TextInputWrap::TextInputWrap(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		TextWidget(c, desc),
		List(c, puu::forest(), true)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "text"){
			this->setText(unikod::toUtf32(get_property_value(p).to_string()));
		}
	}
}
