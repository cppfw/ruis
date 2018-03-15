#include "TextInputArea.hpp"

#include "TextInputLine.hpp"


using namespace morda;

size_t TextInputArea::LinesProvider::count() const noexcept{
	auto ret = this->lines.size();
	if(ret == 0){
		return 1;
	}
	return ret;
}

std::shared_ptr<Widget> TextInputArea::LinesProvider::getWidget(size_t index) {
	auto ret = std::make_shared<TextInputLine>(nullptr);
	
	if(this->lines.size() != 0){
		ASSERT(index < this->lines.size())
		ret->setText(std::u32string(this->lines[index]));
	}else{
		ASSERT(index == 0)
	}
	ret->textChanged = [this, index](TextWidget& w){
		if(this->lines.size() == 0){
			ASSERT(index == 0)
			this->lines.emplace_back(std::u32string());
		}
		
		ASSERT(index < this->lines.size())
		this->lines[index] = w.text();
	};
	
	return ret;
}



TextInputArea::TextInputArea(const stob::Node* chain) :
		Widget(chain),
		ScrollArea(nullptr)
{
	ASSERT(this->list)
	this->add(this->list);
	
	auto& lp = this->getLayoutParams(*this->list);
	
	lp.dim.y = Widget::LayoutParams::fill_c;
	lp.dim.x = Widget::LayoutParams::min_c;
	
	ASSERT(this->linesProvider)
	this->list->setItemsProvider(this->linesProvider);
}
