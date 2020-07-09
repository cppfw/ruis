#pragma once

#include "../widgets/widget.hpp"

#include <set>

namespace morda{

class weak_widget_set{
	typedef std::weak_ptr<widget> pointer_type;
	std::set<pointer_type, std::owner_less<pointer_type>> set;
public:
	weak_widget_set(){}

	weak_widget_set(decltype(set)&& set) :
			set(std::move(set))
	{}

	void add(decltype(set)::value_type w);

	void foreach(const std::function<void(morda::widget&)>& do_this);

	void set_enabled(bool enabled);
};

}
