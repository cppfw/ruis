#pragma once

#include "../widget.hpp"
#include "../group/List.hpp"
#include "../base/text_widget.hpp"

namespace morda{

//TODO: remove?
class TextInputWrap :
		public virtual Widget,
		public text_widget,
		private List
{
public:
	TextInputWrap(std::shared_ptr<morda::context> c, const puu::forest& desc);

	TextInputWrap(const TextInputWrap&) = delete;
	TextInputWrap& operator=(const TextInputWrap&) = delete;

private:

};

}
