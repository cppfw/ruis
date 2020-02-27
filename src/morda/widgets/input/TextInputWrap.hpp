#pragma once

#include "../widget.hpp"
#include "../group/list.hpp"
#include "../base/text_widget.hpp"

namespace morda{

//TODO: remove?
class TextInputWrap :
		public virtual widget,
		public text_widget,
		private list_widget
{
public:
	TextInputWrap(std::shared_ptr<morda::context> c, const puu::forest& desc);

	TextInputWrap(const TextInputWrap&) = delete;
	TextInputWrap& operator=(const TextInputWrap&) = delete;

private:

};

}
