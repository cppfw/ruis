#pragma once

#include "../widget.hpp"
#include "../group/column.hpp"
#include "../group/pile.hpp"

namespace morda{

class collapse_area :
		virtual public widget,
		private column
{
	std::shared_ptr<pile> contentArea;
	std::shared_ptr<pile> title_v;
public:
	collapse_area(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	pile& content()noexcept{
		ASSERT(this->contentArea)
		return *this->contentArea;
	}

	pile& title()noexcept{
		ASSERT(this->title_v)
		return *this->title_v;
	}
};

}
