#pragma once

#include "../Widget.hpp"
#include "../group/List.hpp"
#include "../base/TextWidget.hpp"

namespace morda{

class TextInputWrap :
		public virtual Widget,
		public TextWidget,
		private List
{
	std::vector<std::u32string> lines;
	
public:
	TextInputWrap(const stob::Node* chain);
	
	TextInputWrap(const TextInputWrap&) = delete;
	TextInputWrap& operator=(const TextInputWrap&) = delete;
	
	void setText(std::u32string&& text) override;
	
	std::u32string getText() const override;

	
private:

};

}
