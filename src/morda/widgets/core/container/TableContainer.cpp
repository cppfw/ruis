#include <vector>

#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		Vertical(chain)
{}

void TableContainer::updateRowsLayoutParam(const morda::Vec2r& constraint)const{
	std::vector<std::tuple<TableRow*, morda::Widget::T_ChildrenList::const_iterator, const TableRow::LayoutParams*>> iterators;
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
			lpptr = &tr->getTableRowLayoutParams(**iter);
			
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
			
			utki::clampBottom(maxDimX, (*iter)->measure(d).x);
			utki::clampBottom(maxWeight, lpptr->weight);
		}

		for(auto& i : iterators){
			auto tr = std::get<0>(i);
			auto& iter = std::get<1>(i);
			auto& lpptr = std::get<2>(i);

			if(iter == tr->children().end()){
				break;
			}
			
			ASSERT(lpptr)
			lpptr->processedParams.dim.x = maxDimX;
			lpptr->processedParams.dim.y = lpptr->dim.y;
			lpptr->processedParams.weight = maxWeight;
			++iter;
		}
	}
}



void TableContainer::layOut(){
	this->updateRowsLayoutParam(this->rect().d);
	this->Vertical::layOut();
}


morda::Vec2r TableContainer::measure(const morda::Vec2r& quotum) const{
	this->updateRowsLayoutParam(quotum);
	return this->Vertical::measure(quotum);
}



const Widget::LayoutParams& TableContainer::getLayoutParamsDuringLayout(const Widget& w)const{
	auto& layoutParams = dynamic_cast<const LayoutParams&>(this->getLayoutParams(w));
	auto& lp = const_cast<LayoutParams&>(layoutParams);
	
	lp.dim.x = LayoutParams::D_Max;
	
	return lp;
}
