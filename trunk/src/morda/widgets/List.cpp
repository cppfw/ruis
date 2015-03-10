#include "List.hpp"

#include "../App.hpp"


using namespace morda;




namespace{

class StaticProvider : public List::ItemsProvider{
	std::vector<std::shared_ptr<Widget>> widgets;
public:

	size_t count() const NOEXCEPT override{
		return this->widgets.size();
	}
	
	std::shared_ptr<Widget> getWidget(size_t index)override{
		return this->widgets[index];
	}
	
	void add(std::shared_ptr<Widget> w){
		this->widgets.push_back(std::move(w));
	}
};

}




List::List(bool isVertical, const stob::Node* chain):
		Widget(chain),
		isVertical(isVertical)
{
	if(!chain){
		return;
	}
	
	const stob::Node* n = chain->ThisOrNextNonProperty().node();
	
	if(!n){
		return;
	}
	
	std::shared_ptr<StaticProvider> p = ting::New<StaticProvider>();
	
	for(; n; n = n->NextNonProperty().node()){
		p->add(morda::App::Inst().inflater.Inflate(*n));
	}
	
	this->setItemsProvider(std::move(p));
}



void List::OnResize() {
	this->updateChildrenList();
	
	//TODO:
}


void List::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(this->provider){
		this->provider->list = nullptr;
	}
	this->provider = std::move(provider);
	if(this->provider){
		this->provider->list = this;
	}
	this->notifyDataSetChanged();
}



real List::scrollFactor()const NOEXCEPT{
	//TODO:
	return real(this->posIndex) / real(this->count() - this->visibleCount());
}


void List::setScrollPosAsFactor(real factor){
	this->posIndex = factor * real(this->count());
	
	real intFactor = real(this->posIndex) / real(this->count());
	
	if(this->Children().size() != 0){
		this->posOffset = this->Children().front()->Rect().d.y * ((factor - intFactor) * real(this->count()));
	}else{
		this->posOffset = 0;
	}
	
	//TODO: clamp
	
	this->SetRelayoutNeeded();
}

void List::updateChildrenList(){
	if(this->posIndex >= this->count()){
		this->posIndex = 0;
		this->posOffset = 0;
		this->removeAll();
		return;
	}
	
	//remove unnecessary widgets
	if(this->Children().size() != 0){
		if(this->addedIndex < this->posIndex){
			for(; this->posIndex != this->addedIndex && this->Children().size() != 0; ++this->addedIndex){
				this->Remove(*this->Children().front());
			}
		}
	}
	
	this->addWidgetsAbove();
	
	//TODO:
}



void List::addWidgetsAbove(){
	if(!this->provider){
		return;
	}
	
	real pos;
	
	if(this->isVertical){
		pos = this->Rect().d.y + this->posOffset;
	}else{
		pos = -this->posOffset;
	}
	
	for(; this->addedIndex > this->posIndex; --this->addedIndex){
		auto w = this->provider->getWidget(this->addedIndex - 1);
		
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(*w);
		
		Vec2r dim = this->dimForWidget(*w, lp);
		
		w->Resize(dim);
		
		if(this->isVertical){
			w->MoveTo(Vec2r(0, pos - w->Rect().d.y));
			pos -= w->Rect().d.y;
			
			if(pos < this->Rect().d.y){
				this->Add(w, true);
			}
		}else{
			w->MoveTo(Vec2r(pos, 0));
			pos += w->Rect().d.x;
			
			if(pos > 0){
				this->Add(w, true);
			}
		}
		//TODO: check that need to add more
	}
}
