#pragma once

#include "../widgets/widget.hpp"

#include <set>

#include <utki/span.hpp>

namespace morda{

class widget_set{
	std::set<std::shared_ptr<widget>> set;
public:
	widget_set(){}

	widget_set(decltype(widget_set::set)&& set) :
			set(std::move(set))
	{}

	void add(std::shared_ptr<widget> w);

	void foreach(const std::function<void(morda::widget&)>& do_this);

	void set_enabled(bool enabled);
};

}
