#pragma once

#include "../widget.hpp"
#include "../group/Column.hpp"
#include "../group/Pile.hpp"

namespace morda{
class CollapseArea :
		virtual public Widget,
		private Column
{
	std::shared_ptr<Pile> contentArea;
	std::shared_ptr<Pile> title_v;
public:
	CollapseArea(const puu::forest& desc);

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
