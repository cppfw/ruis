#pragma once

#include "../group/pile.hpp"

namespace morda{

class min_proxy : public morda::pile{
	mutable std::weak_ptr<const morda::widget> target;
	mutable std::string root_id;
	mutable std::vector<std::string> target_id;

public:
	min_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	vector2 measure(const vector2& quotum)const override;
};

}
