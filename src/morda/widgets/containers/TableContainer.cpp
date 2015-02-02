#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain)
{
	//TODO: add rows from STOB chain
}



void TableContainer::OnResize(){
	//TODO:
	
	this->VerticalContainer::OnResize();
}
