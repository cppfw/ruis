#include "Resource.hpp"


using namespace morda;



Resource::~Resource()throw(){
//	TRACE(<< "Resource::~Resource(): invoked" << std::endl)
	if(ting::Ref<ResMapRC> resMap = this->rm){ //if Resource map still exists
//		TRACE(<< "Resource::~Resource(): removing resource" << std::endl)
		//NOTE: the weak ref in the map is still valid because RefCounted destructor is not
		//executed so far. It will be executed right after this destructor.
		resMap->rm.erase(this->it);
//		TRACE(<< "Resource::~Resource(): resource removed" << std::endl)
	}else{
//		TRACE(<< "Resource::~Resource(): resource map is no longer existent" << std::endl)
	}
}
