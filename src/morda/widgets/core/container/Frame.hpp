/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once


#include "Container.hpp"


namespace morda{



class Frame : public Container{
	
	Frame(const Frame&) = delete;
	Frame& operator=(const Frame&) = delete;
	
public:
	Frame(const stob::Node* chain = nullptr);
	
public:	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	void layOut() override;
};


}
