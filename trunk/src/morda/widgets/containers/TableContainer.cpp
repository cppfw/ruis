#include <vector>

#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		VerticalContainer(chain)
{}



void TableContainer::OnResize(){
	std::vector<morda::Widget::T_ChildrenList::const_iterator> iterators;
	iterators.reserve(this->Children().size());
	
	for(auto& c : this->Children()){
		auto tr = dynamic_cast<TableRow*>(c.get());
		if(!tr){
			throw morda::Exc("TableContainer: non-TableRow child found, TableContainer can only hold TableRow children");
		}
		
		iterators.push_back(tr->Children().begin());
	}
	
	//TODO:
	
	this->VerticalContainer::OnResize();
}
