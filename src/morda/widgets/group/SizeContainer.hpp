#pragma once

#include "../container.hpp"

namespace morda{

class SizeContainer : public Container{
public:
	SizeContainer(const stob::Node* chain);

	SizeContainer(const SizeContainer&) = delete;
	SizeContainer& operator=(const SizeContainer&) = delete;


	void lay_out() override;

private:

};

}
