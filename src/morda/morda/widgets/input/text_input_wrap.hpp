#pragma once

#include "../widget.hpp"
#include "../group/list.hpp"
#include "../base/text_widget.hpp"

namespace morda{

//TODO: remove?
class text_input_wrap :
		public virtual widget,
		public text_widget,
		private list_widget
{
public:
	text_input_wrap(std::shared_ptr<morda::context> c, const puu::forest& desc);

	text_input_wrap(const text_input_wrap&) = delete;
	text_input_wrap& operator=(const text_input_wrap&) = delete;

private:

};

}
