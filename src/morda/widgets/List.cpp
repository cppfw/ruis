#include "List.hpp"


using namespace morda;



List::List(bool isVertical, const stob::Node* chain):
		Widget(chain),
		isVertical(isVertical)
{
	//TODO:
}



void List::OnResize() {
	//TODO:
	
	this->Container::OnResize();
}

