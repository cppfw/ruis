#include "TextInputArea.hpp"


using namespace morda;

size_t TextInputArea::LinesProvider::count() const noexcept{
	auto ret = this->tia.lines.size();
	if(ret == 0){
		return 1;
	}
	return ret;
}

std::shared_ptr<Widget> TextInputArea::LinesProvider::getWidget(size_t index) {
	auto ret = utki::makeShared<TextInputLine>();
	
	if(this->tia.lines.size() != 0){
		ASSERT(index < this->tia.lines.size())
		ret->setText(std::u32string(this->tia.lines[index]));
	}else{
		ASSERT(index == 0)
	}
	ret->textChanged = [this, index](SingleLineTextWidget& w){
		ASSERT(index < this->tia.lines.size())
		this->tia.lines[index] = w.text();
	};
	
	return ret;
}



TextInputArea::TextInputArea(const stob::Node* chain) :
		Widget(chain),
		List(true, nullptr)
{
	
}