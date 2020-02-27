#pragma once

#include "../widget.hpp"
#include "../group/Column.hpp"
#include "../group/Pile.hpp"

namespace morda{

class collapse_area :
		virtual public widget,
		private Column
{
	std::shared_ptr<Pile> contentArea;
	std::shared_ptr<Pile> title_v;
public:
	collapse_area(std::shared_ptr<morda::context> c, const puu::forest& desc);

	Pile& content()noexcept{
		ASSERT(this->contentArea)
		return *this->contentArea;
	}

	Pile& title()noexcept{
		ASSERT(this->title_v)
		return *this->title_v;
	}
};

}
