#pragma once

#include "../container.hpp"

namespace morda{

class size_container : public container{
public:
	size_container(std::shared_ptr<morda::context> c, const puu::forest& desc);

	size_container(const size_container&) = delete;
	size_container& operator=(const size_container&) = delete;


	void lay_out() override;

private:

};

}
