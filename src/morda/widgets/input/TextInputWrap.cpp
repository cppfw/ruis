#include "TextInputWrap.hpp"

#include "../../util/util.hpp"

using namespace morda;


TextInputWrap::TextInputWrap(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain),
		List(true, nullptr)
{
	if(auto p = getProperty(chain, "text")){
		this->setText(unikod::toUtf32(p->value()));
	}
}

void TextInputWrap::setText(std::u32string&& text) {
	this->lines.clear();
	this->lines.emplace_back(std::move(text));
	this->onTextChanged();
	this->setRelayoutNeeded();
}

std::u32string TextInputWrap::getText() const {
	std::u32string ret;
	for(auto& l : this->lines){
		ret.append(l);
	}
	return ret;
}
