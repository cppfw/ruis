#include "TextInputWrap.hpp"

#include "../../util/util.hpp"

using namespace morda;


TextInputWrap::TextInputWrap(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain),
		List(nullptr, true)
{
	if(auto p = getProperty(chain, "text")){
		this->setText(unikod::toUtf32(p->value()));
	}
}
