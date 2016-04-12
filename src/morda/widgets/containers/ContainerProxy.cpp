/* 
 * File:   ContainerProxy.cpp
 * Author: ivan
 * 
 * Created on April 12, 2016, 4:50 PM
 */

#include "ContainerProxy.hpp"
#include "../../util/util.hpp"


using namespace morda;


/*
ContainerProxy{
    contents{
		FrameContainer{
		   name{blah_blah}
		}
    }
    forwardTo{blah_blah}
}
*/

ContainerProxy::ContainerProxy(const stob::Node* chain) :
		Widget(chain),
		FrameContainer(getProperty(chain, "contents"))
{
	
	
	//TODO:
}