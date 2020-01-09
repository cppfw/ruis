#pragma once

#include "../container.hpp"

namespace morda{

class SizeContainer : public Container{
public:
	SizeContainer(const puu::forest& desc);

	SizeContainer(const SizeContainer&) = delete;
	SizeContainer& operator=(const SizeContainer&) = delete;


	void lay_out() override;

private:

};

}
