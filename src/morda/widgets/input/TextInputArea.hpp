#pragma once

#include "TextInputLine.hpp"
#include "../group/List.hpp"

namespace morda{

class TextInputArea :
		private List,
		virtual public Widget
{
	class LinesProvider : public List::ItemsProvider{
	public:
		std::vector<std::u32string> lines;

		size_t count() const noexcept override;

		std::shared_ptr<Widget> getWidget(size_t index) override;
	} linesProvider;
	
public:
	TextInputArea(const stob::Node* chain);
	
	TextInputArea(const TextInputArea&) = delete;
	TextInputArea& operator=(const TextInputArea&) = delete;
	
	
private:

};

}
