#include "List.hpp"

#include "../../context.hpp"

//TODO: remove
#include "../../gui.hpp"


using namespace morda;




namespace{

class StaticProvider : public List::ItemsProvider{
	std::vector<puu::tree> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}

	std::shared_ptr<Widget> getWidget(size_t index)override{
//		TRACE(<< "StaticProvider::getWidget(): index = " << index << std::endl)
		auto i = std::next(this->widgets.begin(), index);
		ASSERT(this->get_list())
		return this->get_list()->context->inflater.inflate(i, i + 1);
	}


	void recycle(size_t index, std::shared_ptr<Widget> w)override{
//		TRACE(<< "StaticProvider::recycle(): index = " << index << std::endl)
	}


	void add(puu::tree&& w){
		this->widgets.emplace_back(std::move(w));
	}
};

}




List::List(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical):
		widget(std::move(c), desc),
		container(this->context, puu::forest()),
		OrientedWidget(this->context, puu::forest(), vertical)
{
	std::shared_ptr<StaticProvider> pr = std::make_shared<StaticProvider>();

	for(const auto& p : desc){
		if(is_property(p)){
			continue;
		}

		pr->add(puu::tree(p));
	}

	this->setItemsProvider(std::move(pr));
}



void List::lay_out() {
//	TRACE(<< "List::lay_out(): invoked" << std::endl)

	this->numTailItems = 0;//means that it needs to be recomputed

	this->updateChildrenList();
}


void List::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(provider && provider->list){
		throw utki::invalid_state("given provider is already set to some List");
	}

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

	if(this->children().size() == 0){
		return 0;
	}

	ASSERT(this->provider->count() >= this->numTailItems)

	size_t length = this->provider->count() - this->numTailItems;

	if(length == 0){
		return 0;
	}

	if(this->numTailItems == 0){
		return 0;
	}

	real d = this->rect().d[this->getLongIndex()];

	ASSERT(this->numTailItems != 0)
	d = (d + this->firstTailItemOffset) / this->numTailItems;

	if(d <= 0){
		return 0;
	}

	return (real(this->posIndex) * d + this->posOffset) / (real(length) * d + this->firstTailItemOffset);
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

		if(this->children().size() != 0){
			real d = this->rect().d[this->getLongIndex()];
			d = (d + this->firstTailItemOffset) / this->numTailItems;

			this->posOffset = ::round(d * (factor - intFactor) * real(this->provider->count() - this->numTailItems) + factor * this->firstTailItemOffset);
		}else{
			this->posOffset = 0;
		}
	}else{
		ASSERT(this->posIndex == 0)
		this->posOffset = ::round(factor * this->firstTailItemOffset);
	}

	this->updateChildrenList();
}

// TODO: refactor
bool List::arrangeWidget(std::shared_ptr<widget>& w, real& pos, bool added, size_t index, list::const_iterator& insertBefore){
	auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);

	Vec2r dim = this->dimForWidget(*w, lp);

	w->resize(dim);

	unsigned longIndex = this->getLongIndex();
	unsigned transIndex = this->getTransIndex();

	{
		Vec2r to;
		to[longIndex] = pos;
		to[transIndex] = 0;
		w->move_to(to);
	}
	pos += w->rect().d[longIndex];

	if(pos > 0){
		if(!added){
			insertBefore = this->insert(w, insertBefore);
		}
		++insertBefore;
		if(this->addedIndex > index){
			this->addedIndex = index;
		}
	}else{
		++this->posIndex;
		this->posOffset -= w->rect().d[longIndex];
		if(added){
			insertBefore = this->erase(insertBefore);
			if(this->provider){
				this->provider->recycle(index, w);
			}
			++this->addedIndex;
		}else{
			if(this->provider){
				this->provider->recycle(index, w);
			}
		}
	}

	if(w->rect().p[longIndex] + w->rect().d[longIndex] >= this->rect().d[longIndex]){
		return true;
	}

	return false;
}

// TODO: refactor
void List::updateChildrenList(){
	if(!this->provider){
		this->posIndex = 0;
		this->posOffset = 0;

		this->clear();
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

	real pos = -this->posOffset;

//	TRACE(<< "List::updateChildrenList(): this->addedIndex = " << this->addedIndex << " this->posIndex = " << this->posIndex << std::endl)

	// remove widgets from top
	for(; this->children().size() != 0 && this->addedIndex < this->posIndex; ++this->addedIndex){
		auto i = this->children().begin();
		auto w = *i;
		this->erase(i);
		if(this->provider){
			this->provider->recycle(this->addedIndex, w);
		}
	}

	auto iter = this->children().begin();
	size_t iterIndex = this->addedIndex;
	const size_t iterEndIndex = iterIndex + this->children().size();
	size_t index = this->posIndex;
	for(bool is_last = false; index < this->provider->count() && !is_last;){
		std::shared_ptr<widget> w;
		bool isAdded;
		if(iterIndex <= index && index < iterEndIndex && iter != this->children().end()){
			w = *iter;
			++iterIndex;
			isAdded = true;
		}else{
			w = this->provider->getWidget(index);
			isAdded = false;
		}

		is_last = this->arrangeWidget(w, pos, isAdded, index, iter);
		++index;
	}

	// remove rest
	if(iterIndex < iterEndIndex){
		ASSERT(iter != this->children().end())
		for(; iter != this->children().end(); ++iterIndex){
			auto w = *iter;
			iter = this->erase(iter);
			if(this->provider){
				this->provider->recycle(iterIndex, w);
			}
		}
	}
}




void List::updateTailItemsInfo(){
	this->numTailItems = 0;

	if(!this->provider || this->provider->count() == 0){
		return;
	}

	unsigned longIndex = this->getLongIndex();

	real dim = this->rect().d[longIndex];

	ASSERT(this->provider)
	ASSERT(this->provider->count() > 0)

	for(size_t i = this->provider->count(); i != 0 && dim > 0; --i){
		++this->numTailItems;

		auto w = this->provider->getWidget(i - 1);
		ASSERT(w)

		auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);

		Vec2r d = this->dimForWidget(*w, lp);

		dim -= d[longIndex];
	}

	this->firstTailItemIndex = this->provider->count() - this->numTailItems;

	if(dim > 0){
		this->firstTailItemOffset = -1;
	}else{
		this->firstTailItemOffset = -dim;
	}
}

void List::scrollBy(real delta) {
	if(!this->provider){
		return;
	}

	unsigned longIndex = this->getLongIndex();
//	unsigned transIndex;

//	TRACE(<< "delta = " << delta << std::endl)

	if(delta >= 0){
		for(auto& c : this->children()){
			auto wd = c->rect().d[longIndex] - this->posOffset;
			if(wd > delta){
				this->posOffset += delta;
				delta -= wd;
				break;
			}

			delta -= wd;
			this->posOffset = 0;
			++this->posIndex;
		}

		if(delta > 0){
			ASSERT_INFO(
					this->posIndex > this->addedIndex + this->children().size(),
					"this->posIndex = " << this->posIndex
					<< " this->addedIndex = " << this->addedIndex
					<< " this->children().size() = " << this->children().size()
				)
			for(; this->posIndex < this->firstTailItemIndex;){
				auto w = this->provider->getWidget(this->posIndex);
				auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);
				Vec2r d = this->dimForWidget(*w, lp);
				this->push_back(w); //this is just optimization, to avoid creating same widget twice
				if(d[longIndex] > delta){
					this->posOffset = delta;
					break;
				}
				delta -= d[longIndex];
				ASSERT(this->posOffset == 0)
				++this->posIndex;
			}
		}
	}else{
		delta = -delta;
		if(delta <= this->posOffset){
			this->posOffset -= delta;
		}else{
			ASSERT(this->addedIndex == this->posIndex)
			delta -= this->posOffset;
			this->posOffset = 0;
			for(; this->posIndex > 0;){
				ASSERT(this->addedIndex == this->posIndex)
				--this->posIndex;
				auto w = this->provider->getWidget(this->posIndex);
				auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);
				Vec2r d = this->dimForWidget(*w, lp);
				this->insert(w, this->children().begin()); //this is just optimization, to avoid creating same widget twice
				--this->addedIndex;
				if(d[longIndex] > delta){
					this->posOffset = d[longIndex] - delta;
					break;
				}
				delta -= d[longIndex];
			}
		}
	}

	this->updateChildrenList();
}

morda::Vec2r List::measure(const morda::Vec2r& quotum) const {
	unsigned longIndex = this->getLongIndex();
	unsigned transIndex = this->getTransIndex();

	Vec2r ret(quotum);

	utki::clampBottom(ret[longIndex], real(0));

	if(ret[transIndex] > 0){
		return ret;
	}

	ret[transIndex] = 0;

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		utki::clampBottom(ret[transIndex], (*i)->rect().d[transIndex]);
	}

	return ret;
}

void List::ItemsProvider::notifyDataSetChanged(){
	if(!this->get_list()){
		return;
	}

	this->get_list()->context->run_from_ui_thread(
		[this](){
			this->list->handleDataSetChanged();
		}
	);
}

void List::handleDataSetChanged(){
	this->numTailItems = 0; //means that it needs to be recomputed

	this->clear();
	this->addedIndex = size_t(-1);

	this->updateChildrenList();

	if(this->dataSetChanged){
		this->dataSetChanged(*this);
	}
}
