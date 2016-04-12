/* 
 * File:   ContainerProxy.hpp
 * Author: ivan
 *
 * Created on April 12, 2016, 4:49 PM
 */

#pragma once

#include "FrameContainer.hpp"

namespace morda{

class ContainerProxy :
		virtual public Widget,
		private FrameContainer
{
public:
	ContainerProxy(const stob::Node* chain = nullptr);
	
	ContainerProxy(const ContainerProxy&) = delete;
	ContainerProxy& operator=(const ContainerProxy&) = delete;
	

private:

};

}
