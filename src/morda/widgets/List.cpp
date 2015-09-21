#include "List.hpp"

#include "../App.hpp"


using namespace morda;




namespace{

class StaticProvider : public List::ItemsProvider{
	std::vector<std::unique_ptr<stob::Node>> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}
	
	std::shared_ptr<Widget> getWidget(size_t index)override{
//		TRACE(<< "StaticProvider::getWidget(): index = " << index << std::endl)
		return morda::App::Inst().inflater.Inflate(*(this->widgets[index]));
	}
	

	void recycle(size_t index, std::shared_ptr<Widget> w)override{
//		TRACE(<< "StaticProvider::recycle(): index = " << index << std::endl)
	}

	
	void add(std::unique_ptr<stob::Node> w){
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
		p->add(n->Clone());
	}
	
	this->setItemsProvider(std::move(p));
}



void List::layOut() {
//	TRACE(<< "List::layOut(): invoked" << std::endl)
	
	this->numTailItems = 0;//means that it needs to be recomputed
	
	this->updateChildrenList();
}


void List::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(this->provider){
		this->provider->list = nullptr;
	}
	this->provider = std::move(provider);
	if(this->provider){
		this->provider->list = this;
	}
	this->handleDataSetChanged();
}



real List::scrollFactor()const noexcept{
	if(!this->provider || this->provider->count() == 0){
		return 0;
	}
	
	if(this->provider->count() == this->visibleCount()){
		return 0;
	}
	
	return real(this->posIndex) / real(this->provider->count() - this->visibleCount());
}


void List::setScrollPosAsFactor(real factor){
	if(!this->provider || this->provider->count() == 0){
		return;
	}
	
	if(this->numTailItems == 0){
		this->updateTailItemsInfo();
	}
	
	this->posIndex = size_t(factor * real(this->provider->count() - this->numTailItems));
	
//	TRACE(<< "List::setScrollPosAsFactor(): this->posIndex = " << this->posIndex << std::endl)
	
	if(this->provider->count() != this->numTailItems){
		real intFactor = real(this->posIndex) / real(this->provider->count() - this->numTailItems);

		if(this->Children().size() != 0){
			real d;
			if(this->isVertical){
				d = this->Children().front()->rect().d.y;
			}else{
				d = this->Children().front()->rect().d.x;
			}
			
			this->posOffset = ting::math::Round(d * (factor - intFactor) * real(this->provider->count() - this->numTailItems) + factor * this->firstTailItemOffset);
		}else{
			this->posOffset = 0;
		}
	}else{
		ASSERT(this->posIndex == 0)
		this->posOffset = ting::math::Round(factor * this->firstTailItemOffset);
	}
	
	this->updateChildrenList();
}

bool List::arrangeWidget(std::shared_ptr<Widget>& w, real& pos, bool added, size_t index, T_ChildrenList::const_iterator insertBefore){
	auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);
		
	Vec2r dim = this->dimForWidget(*w, lp);

	w->resize(dim);

	if(this->isVertical){
		pos -= w->rect().d.y;
		w->moveTo(Vec2r(0, pos));

		if(pos < this->rect().d.y){
			if(!added){
				this->Add(w, insertBefore);
			}
			if(this->addedIndex > index){
				this->addedIndex = index;
			}
		}else{
			++this->posIndex;
			this->posOffset -= w->rect().d.y;
			if(added){
				auto widget = this->Remove(*w);
				if(this->provider){
					this->provider->recycle(index, widget);
				}
				++this->addedIndex;
			}else{
				if(this->provider){
					this->provider->recycle(index, w);
				}
			}
		}

		if(w->rect().p.y <= 0){
			return true;
		}
	}else{
		w->moveTo(Vec2r(pos, 0));
		pos += w->rect().d.x;

		if(pos > 0){
			if(!added){
				this->Add(w, insertBefore);
			}
			if(this->addedIndex > index){
				this->addedIndex = index;
			}
		}else{
			++this->posIndex;
			this->posOffset -= w->rect().d.x;
			if(added){
				auto widget = this->Remove(*w);
				if(this->provider){
					this->provider->recycle(index, widget);
				}
				++this->addedIndex;
			}else{
				if(this->provider){
					this->provider->recycle(index, w);
				}
			}
		}

		if(w->rect().Right() >= this->rect().d.x){
			return true;
		}
	}
	return false;
}


void List::updateChildrenList(){
	if(!this->provider){
		this->posIndex = 0;
		this->posOffset = 0;
		
		this->removeAll();
		this->addedIndex = size_t(-1);
		return;
	}
	
	if(this->numTailItems == 0){
		this->updateTailItemsInfo();
	}
	
	if(this->posIndex == this->firstTailItemIndex){
		if(this->posOffset > this->firstTailItemOffset){
			this->posOffset = this->firstTailItemOffset;
		}
	}else if(this->posIndex > this->firstTailItemIndex){
		this->posIndex = this->firstTailItemIndex;
		this->posOffset = this->firstTailItemOffset;
	}
	
	real pos;
	
	if(this->isVertical){
		pos = this->rect().d.y + this->posOffset;
	}else{
		pos = -this->posOffset;
	}
	
//	TRACE(<< "List::updateChildrenList(): this->addedIndex = " << this->addedIndex << " this->posIndex = " << this->posIndex << std::endl)
	
	//remove widgets from top
	for(; this->Children().size() != 0 && this->addedIndex < this->posIndex; ++this->addedIndex){
		auto w = (*this->Children().begin())->removeFromParent();
		if(this->provider){
			this->provider->recycle(this->addedIndex, w);
		}
	}
	
	auto iter = this->Children().begin();
	size_t iterIndex = this->addedIndex;
	size_t iterEndIndex = iterIndex + this->Children().size();
	size_t index = this->posIndex;
	for(; index < this->provider->count();){
		std::shared_ptr<Widget> w;
		bool isAdded;
		if(iterIndex <= index && index < iterEndIndex && iter != this->Children().end()){
			w = *iter;
			++iter;
			++iterIndex;
			isAdded = true;
		}else{
			w = this->provider->getWidget(index);
			isAdded = false;
		}
		
		if(this->arrangeWidget(w, pos, isAdded, index, iter)){
			++index;
			break;
		}
		++index;
	}
	
	//remove rest
	if(iterIndex < iterEndIndex){
		size_t oldIterIndex = iterIndex;
		for(;;){
			auto i = iter;
			++i;
			++iterIndex;
			if(i == this->Children().end()){
				break;
			}
			auto w = this->Remove(i);
			if(this->provider){
				this->provider->recycle(iterIndex, w);
			}
		}
		auto w = this->Remove(iter);
		if(this->provider){
			this->provider->recycle(oldIterIndex, w);
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
		dim = this->rect().d.y;
	}else{
		dim = this->rect().d.x;
	}
	
	ASSERT(this->provider)
	ASSERT(this->provider->count() > 0)
	
	for(size_t i = this->provider->count(); i != 0 && dim > 0; --i){
		++this->numTailItems;
		
		auto w = this->provider->getWidget(i - 1);
		ASSERT(w)
		
		auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);
		
		Vec2r d = this->dimForWidget(*w, lp);
		
		if(this->isVertical){
			dim -= d.y;
		}else{
			dim -= d.x;
		}
	}
	
	this->firstTailItemIndex = this->provider->count() - this->numTailItems;
	
	if(dim > 0){
		this->firstTailItemOffset = -1;
	}else{
		this->firstTailItemOffset = -dim;
	}
}

morda::Vec2r List::measure(const morda::Vec2r& quotum) const {
	unsigned longIndex, transIndex;
	
	if(this->isVertical){
		longIndex = 1;
		transIndex = 0;
	}else{
		longIndex = 0;
		transIndex = 1;
	}
	
	Vec2r ret(quotum);
	
	ting::util::ClampBottom(ret[longIndex], real(0));
	
	if(ret[transIndex] > 0){
		return ret;
	}
	
	ret[transIndex] = 0;
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		ting::util::ClampBottom(ret[transIndex], (*i)->rect().d[transIndex]);
	}
	
	return ret;
}

void List::ItemsProvider::notifyDataSetChanged() {
	if (!this->list) {
		return;
	}
	
	App::Inst().PostToUIThread_ts(
		[this](){
			this->list->handleDataSetChanged();
		}
	);
}
