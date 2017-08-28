#pragma once

#include "core/Widget.hpp"
#include "core/container/LinearContainer.hpp"
#include "core/container/Pile.hpp"

namespace morda{
class CollapseArea :
		virtual public Widget,
		private Column
{
	std::shared_ptr<Pile> contentArea;
public:
	CollapseArea(const stob::Node* chain = nullptr);
	
	Pile& content()noexcept{
		ASSERT(this->contentArea)
		return *this->contentArea;
	}
};
}
