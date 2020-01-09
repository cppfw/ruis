#pragma once

#include "../container.hpp"

namespace morda{

class SizeContainer : public Container{
public:
	SizeContainer(const puu::forest& desc);
	SizeContainer(const stob::Node* chain) : SizeContainer(stob_to_puu(chain)){}

	SizeContainer(const SizeContainer&) = delete;
	SizeContainer& operator=(const SizeContainer&) = delete;


	void lay_out() override;

private:

};

}
