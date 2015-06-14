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
//		TRACE(<< "StaticProvider::getWidget(): index = " << index << std::endl)
		//TODO: inflate new widget each time
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
	this->numTailItems = 0;//indicate that number of tail items has to be recomputed
	
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
//	return this->factor;
}


void List::setScrollPosAsFactor(real factor){
	this->factor = factor;

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
	
	//TODO:
	
	
	this->removeAll();
	this->addedIndex = 0;
	
	if(!this->provider){
		this->posIndex = 0;
		this->posOffset = 0;
		return;
	}
	
	real pos;
	
	if(this->isVertical){
		pos = this->Rect().d.y + this->posOffset;
	}else{
		pos = -this->posOffset;
	}
	
	for(size_t i = this->posIndex; i < this->count(); ++i){
		auto w = this->provider->getWidget(i);
		
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(*w);
		
		Vec2r dim = this->dimForWidget(*w, lp);
		
		w->Resize(dim);
		
		if(this->isVertical){
			w->MoveTo(Vec2r(0, pos - w->Rect().d.y));
			pos -= w->Rect().d.y;
			
			if(pos < this->Rect().d.y){
				this->Add(w);
			}
			
			if(w->Rect().p.y <= 0){
				break;
			}
		}else{
			w->MoveTo(Vec2r(pos, 0));
			pos += w->Rect().d.x;
			
			if(pos > 0){
				this->Add(w);
			}
			
			if(w->Rect().Right() >= this->Rect().d.x){
				break;
			}
		}
	}
}



void List::updateTailItemsInfo(){
	this->numTailItems = 0;
	
	if(!this->provider || this->provider->count() == 0){
		return;
	}
	
	real dim;
	
	if(this->isVertical){
		dim = this->Rect().d.y;
	}else{
		dim = this->Rect().d.x;
	}
	
	ASSERT(this->provider)
	ASSERT(this->provider->count() > 0)
	
	for(size_t i = this->provider->count(); i != 0 && dim > 0; --i){
		++this->numTailItems;
		
		auto w = this->provider->getWidget(i - 1);
		
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(*w);
		
		Vec2r d = this->dimForWidget(*w, lp);
		
		if(this->isVertical){
			dim -= d.y;
		}else{
			dim -= d.x;
		}
	}
	
	if(dim > 0){
		this->firstTailItemOffset = -1;
	}else{
		this->firstTailItemOffset = -dim;
	}
}
