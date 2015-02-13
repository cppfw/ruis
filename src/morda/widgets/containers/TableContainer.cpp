#include <vector>

#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		VerticalContainer(chain)
{ }

void TableContainer::UpdateRowsLayoutParam()const{
	std::vector<std::tuple<TableRow*, morda::Widget::T_ChildrenList::const_iterator, TableRow::LayoutParams*>> iterators;
	iterators.reserve(this->Children().size());
	
	for(auto& c : this->Children()){
		auto tr = dynamic_cast<TableRow*>(c.get());
		if(!tr){
			throw morda::Exc("TableContainer: non-TableRow child found, TableContainer can only hold TableRow children");
		}
		
		iterators.push_back(std::make_tuple(tr, tr->Children().begin(), nullptr));
	}
	
	for(bool notEnd = true; notEnd;){
		real maxDimX = 0;
		real maxWeight = 0;
		
		notEnd = false;
		
		for(auto& i : iterators){
			auto tr = std::get<0>(i);
			auto& iter = std::get<1>(i);
			auto& lpptr = std::get<2>(i);

			if(iter == tr->Children().end()){
				break;
			}
			
			notEnd = true;
			lpptr = &tr->GetTableRowLayoutParams(**iter);
			real x = lpptr->dim.x < 0 ? (*iter)->measure(Vec2r(-1)).x : lpptr->dim.x;
			ting::util::ClampBottom(maxDimX, x);
			ting::util::ClampBottom(maxWeight, lpptr->weight);
		}

		for(auto& i : iterators){
			auto tr = std::get<0>(i);
			auto& iter = std::get<1>(i);
			auto& lpptr = std::get<2>(i);

			if(iter == tr->Children().end()){
				break;
			}
			
			ASSERT(lpptr)
			lpptr->modifiedParams.fill = lpptr->fill;
			lpptr->modifiedParams.dim.x = maxDimX;
			lpptr->modifiedParams.dim.y = lpptr->dim.y;
			lpptr->modifiedParams.weight = maxWeight;
			++iter;
		}
	}
}



void TableContainer::OnResize(){
	this->UpdateRowsLayoutParam();
	this->VerticalContainer::OnResize();
}


morda::Vec2r TableContainer::onMeasure(const morda::Vec2r& quotum) const NOEXCEPT{
	this->UpdateRowsLayoutParam();
	return this->VerticalContainer::onMeasure(quotum);
}


morda::LayoutParams& TableContainer::GetLayoutParams(Widget& w){
	auto& layoutParams = dynamic_cast<LayoutParams&>(this->VerticalContainer::GetLayoutParams(w));
	
	layoutParams.fill.x = true;
	
	return layoutParams;
}
