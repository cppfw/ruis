#pragma once

#include "../Container.hpp"

namespace morda{

class SizeContainer : public Container{
public:
	SizeContainer(const stob::Node* chain);
	
	SizeContainer(const SizeContainer&) = delete;
	SizeContainer& operator=(const SizeContainer&) = delete;
	

	void layOut() override;

private:

};

}
