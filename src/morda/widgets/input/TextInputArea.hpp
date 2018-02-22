#pragma once

#include "TextInputLine.hpp"
#include "../group/List.hpp"

namespace morda{

class TextInputArea :
		private List,
		virtual public Widget
{
	class LinesProvider : public List::ItemsProvider{
		TextInputArea& tia;
	public:
		LinesProvider(TextInputArea& tia) :
				tia(tia)
		{}

		size_t count() const noexcept override;

		std::shared_ptr<Widget> getWidget(size_t index) override;
	};
	
	std::vector<std::u32string> lines{std::u32string()};
	
public:
	TextInputArea(const stob::Node* chain = nullptr);
	
	TextInputArea(const TextInputArea&) = delete;
	TextInputArea& operator=(const TextInputArea&) = delete;
	
	
private:

};

}
