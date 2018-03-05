#include "TextInputArea.hpp"


using namespace morda;

size_t TextInputArea::LinesProvider::count() const noexcept{
	auto ret = this->lines.size();
	if(ret == 0){
		return 1;
	}
	return ret;
}

std::shared_ptr<Widget> TextInputArea::LinesProvider::getWidget(size_t index) {
	auto ret = std::make_shared<TextInputLine>();
	
	if(this->lines.size() != 0){
		ASSERT(index < this->lines.size())
		ret->setText(std::u32string(this->lines[index]));
	}else{
		ASSERT(index == 0)
	}
	ret->textChanged = [this, index](SingleLineTextWidget& w){
		ASSERT(index < this->lines.size())
		this->lines[index] = w.text();
	};
	
	return ret;
}



TextInputArea::TextInputArea(const stob::Node* chain) :
		Widget(chain),
		List(true, nullptr)
{
	
}
