#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		VerticalContainer(chain)
{}



void TableContainer::OnResize(){
	//TODO:
	
	this->VerticalContainer::OnResize();
}
