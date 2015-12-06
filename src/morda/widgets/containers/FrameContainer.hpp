/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once


#include "Container.hpp"


namespace morda{



class FrameContainer : public Container{
	
	FrameContainer(const FrameContainer&) = delete;
	FrameContainer& operator=(const FrameContainer&) = delete;
	
public:
	FrameContainer(const stob::Node* chain = nullptr);
	
public:	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	void layOut() override;
};


}
