#include <vector>

#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		VerticalContainer(chain)
{}

void TableContainer::UpdateRowsLayoutParam(const morda::Vec2r& constraint)const{
	std::vector<std::tuple<TableRow*, morda::Widget::T_ChildrenList::const_iterator, TableRow::LayoutParams*>> iterators;
	iterators.reserve(this->children().size());
	
	for(auto& c : this->children()){
		auto tr = dynamic_cast<TableRow*>(c.get());
		if(!tr){
			throw morda::Exc("TableContainer: non-TableRow child found, TableContainer can only hold TableRow children");
		}
		
		iterators.push_back(std::make_tuple(tr, tr->children().begin(), nullptr));
	}
	
	for(bool notEnd = true; notEnd;){
		real maxDimX = 0;
		real maxWeight = 0;
		
		notEnd = false;
		
		for(auto& i : iterators){
			auto tr = std::get<0>(i);
			auto& iter = std::get<1>(i);
			auto& lpptr = std::get<2>(i);

			if(iter == tr->children().end()){
				break;
			}
			
			notEnd = true;
			lpptr = &tr->GetTableRowLayoutParams(**iter);
			
			Vec2r d;
			
			if(lpptr->dim.x == LayoutParams::D_Max){
				throw morda::Exc("TableContainer::UpdateRowsLayoutParam(): \"max\" in horizontal direction: mistake");
			}
			
			if(lpptr->dim.y == LayoutParams::D_Max){
				if(constraint.y >= 0){
					d.y = constraint.y;
				}else{
					d.y = -1;
				}
			}else if(lpptr->dim.y == LayoutParams::D_Min || lpptr->dim.y < 0){
				d.y = -1;
			}else{
				d.y = lpptr->dim.y;
			}
			
			if(lpptr->dim.x == LayoutParams::D_Min || lpptr->dim.x < 0){
				d.x = -1;
			}else{
				d.x = lpptr->dim.x;
			}
			
			ting::util::ClampBottom(maxDimX, (*iter)->measure(d).x);
			ting::util::ClampBottom(maxWeight, lpptr->weight);
		}

		for(auto& i : iterators){
			auto tr = std::get<0>(i);
			auto& iter = std::get<1>(i);
			auto& lpptr = std::get<2>(i);

			if(iter == tr->children().end()){
				break;
			}
			
			ASSERT(lpptr)
			lpptr->modifiedParams.dim.x = maxDimX;
			lpptr->modifiedParams.dim.y = lpptr->dim.y;
			lpptr->modifiedParams.weight = maxWeight;
			++iter;
		}
	}
}



void TableContainer::layOut(){
	this->UpdateRowsLayoutParam(this->rect().d);
	this->VerticalContainer::layOut();
}


morda::Vec2r TableContainer::measure(const morda::Vec2r& quotum) const{
	this->UpdateRowsLayoutParam(quotum);
	return this->VerticalContainer::measure(quotum);
}


Widget::LayoutParams& TableContainer::getLayoutParams_internal(Widget& w){
	auto& layoutParams = dynamic_cast<LayoutParams&>(this->VerticalContainer::getLayoutParams_internal(w));
	
	layoutParams.dim.x = LayoutParams::D_Max;
	
	return layoutParams;
}
