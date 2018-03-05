#pragma once

#include "../group/List.hpp"
#include "../group/ScrollArea.hpp"

namespace morda{

class TextInputArea :
		//NOTE: order of virtual public and private declarations here matters for clang due to some bug,
		//      see http://stackoverflow.com/questions/42427145/clang-cannot-cast-to-private-base-while-there-is-a-public-virtual-inheritance
		virtual public Widget,
		private ScrollArea
{
	std::shared_ptr<List> list = std::make_shared<VerticalList>();
	
	class LinesProvider : public List::ItemsProvider{
	public:
		std::vector<std::u32string> lines = {{U"wefqw"}};

		size_t count() const noexcept override;

		std::shared_ptr<Widget> getWidget(size_t index) override;
	};
	
	std::shared_ptr<LinesProvider> linesProvider = std::make_shared<LinesProvider>();
	
public:
	TextInputArea(const stob::Node* chain);
	
	TextInputArea(const TextInputArea&) = delete;
	TextInputArea& operator=(const TextInputArea&) = delete;
	
	void setVerticalScrollPosAsFactor(real factor){
		this->list->setScrollPosAsFactor(factor);
	}
	
	void setHorizontalScrollPosAsFactor(real factor){
		this->setScrollPosAsFactor(Vec2r(factor, 0));
	}
	
	Vec2r scrollFactor()const{
		return Vec2r(this->ScrollArea::scrollFactor().x, this->list->scrollFactor());
	}
private:

};

}
