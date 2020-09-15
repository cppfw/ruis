#include "list.hpp"

#include "../../context.hpp"

using namespace morda;

namespace{
class static_provider : public list_widget::provider{
	std::vector<puu::tree> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}

	std::shared_ptr<widget> get_widget(size_t index)override{
//		TRACE(<< "static_provider::getWidget(): index = " << index << std::endl)
		auto i = std::next(this->widgets.begin(), index);
		ASSERT(this->get_list())
		return this->get_list()->context->inflater.inflate(i, i + 1);
	}


	void recycle(size_t index, std::shared_ptr<widget> w)override{
//		TRACE(<< "static_provider::recycle(): index = " << index << std::endl)
	}


	void add(puu::tree&& w){
		this->widgets.emplace_back(std::move(w));
	}
};
}

list_widget::list_widget(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical):
		widget(std::move(c), desc),
		container(this->context, puu::forest()),
		oriented_widget(this->context, puu::forest(), vertical)
{
	std::shared_ptr<static_provider> pr = std::make_shared<static_provider>();

	for(const auto& p : desc){
		if(is_property(p)){
			continue;
		}

		pr->add(puu::tree(p));
	}

	this->set_provider(std::move(pr));
}

void list_widget::lay_out(){
//	TRACE(<< "list_widget::lay_out(): invoked" << std::endl)

	this->numTailItems = 0;//means that it needs to be recomputed

	this->updateChildrenList();
}

void list_widget::set_provider(std::shared_ptr<provider> item_provider){
	if(item_provider && item_provider->parent_list){
		throw std::logic_error("given provider is already set to some list_widget");
	}

	if(this->item_provider){
		this->item_provider->parent_list = nullptr;
	}
	this->item_provider = std::move(item_provider);
	if(this->item_provider){
		this->item_provider->parent_list = this;
	}
	this->handleDataSetChanged();
}

real list_widget::get_scroll_factor()const noexcept{
	if(!this->item_provider || this->item_provider->count() == 0){
		return 0;
	}

	if(this->children().size() == 0){
		return 0;
	}

	ASSERT(this->item_provider->count() >= this->numTailItems)

	size_t length = this->item_provider->count() - this->numTailItems;

	if(length == 0){
		return 0;
	}

	if(this->numTailItems == 0){
		return 0;
	}

	real d = this->rect().d[this->get_long_index()];

	ASSERT(this->numTailItems != 0)
	d = (d + this->firstTailItemOffset) / this->numTailItems;

	if(d <= 0){
		return 0;
	}

	return (real(this->posIndex) * d + this->posOffset) / (real(length) * d + this->firstTailItemOffset);
}

void list_widget::set_scroll_factor(real factor){
	if(!this->item_provider || this->item_provider->count() == 0){
		return;
	}

	if(this->numTailItems == 0){
		this->updateTailItemsInfo();
	}

	this->posIndex = size_t(factor * real(this->item_provider->count() - this->numTailItems));

//	TRACE(<< "list_widget::setScrollPosAsFactor(): this->posIndex = " << this->posIndex << std::endl)

	if(this->item_provider->count() != this->numTailItems){
		real intFactor = real(this->posIndex) / real(this->item_provider->count() - this->numTailItems);

		if(this->children().size() != 0){
			real d = this->rect().d[this->get_long_index()];
			d = (d + this->firstTailItemOffset) / this->numTailItems;

			this->posOffset = ::round(d * (factor - intFactor) * real(this->item_provider->count() - this->numTailItems) + factor * this->firstTailItemOffset);
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
bool list_widget::arrangeWidget(std::shared_ptr<widget>& w, real& pos, bool added, size_t index, widget_list::const_iterator& insertBefore){
	auto& lp = this->get_layout_params_as_const<layout_params>(*w);

	vector2 dim = this->dims_for_widget(*w, lp);

	w->resize(dim);

	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	{
		vector2 to;
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
			if(this->item_provider){
				this->item_provider->recycle(index, w);
			}
			++this->addedIndex;
		}else{
			if(this->item_provider){
				this->item_provider->recycle(index, w);
			}
		}
	}

	if(w->rect().p[longIndex] + w->rect().d[longIndex] >= this->rect().d[longIndex]){
		return true;
	}

	return false;
}

// TODO: refactor
void list_widget::updateChildrenList(){
	if(!this->item_provider){
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

//	TRACE(<< "list_widget::updateChildrenList(): this->addedIndex = " << this->addedIndex << " this->posIndex = " << this->posIndex << std::endl)

	// remove widgets from top
	for(; this->children().size() != 0 && this->addedIndex < this->posIndex; ++this->addedIndex){
		auto i = this->children().begin();
		auto w = *i;
		this->erase(i);
		if(this->item_provider){
			this->item_provider->recycle(this->addedIndex, w);
		}
	}

	auto iter = this->children().begin();
	size_t iterIndex = this->addedIndex;
	const size_t iterEndIndex = iterIndex + this->children().size();
	size_t index = this->posIndex;
	for(bool is_last = false; index < this->item_provider->count() && !is_last;){
		std::shared_ptr<widget> w;
		bool isAdded;
		if(iterIndex <= index && index < iterEndIndex && iter != this->children().end()){
			w = *iter;
			++iterIndex;
			isAdded = true;
		}else{
			w = this->item_provider->get_widget(index);
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
			if(this->item_provider){
				this->item_provider->recycle(iterIndex, w);
			}
		}
	}
}

void list_widget::updateTailItemsInfo(){
	this->numTailItems = 0;

	if(!this->item_provider || this->item_provider->count() == 0){
		return;
	}

	unsigned longIndex = this->get_long_index();

	real dim = this->rect().d[longIndex];

	ASSERT(this->item_provider)
	ASSERT(this->item_provider->count() > 0)

	for(size_t i = this->item_provider->count(); i != 0 && dim > 0; --i){
		++this->numTailItems;

		auto w = this->item_provider->get_widget(i - 1);
		ASSERT(w)

		auto& lp = this->get_layout_params_as_const<layout_params>(*w);

		vector2 d = this->dims_for_widget(*w, lp);

		dim -= d[longIndex];
	}

	this->firstTailItemIndex = this->item_provider->count() - this->numTailItems;

	if(dim > 0){
		this->firstTailItemOffset = -1;
	}else{
		this->firstTailItemOffset = -dim;
	}
}

void list_widget::scroll_by(real delta) {
	if(!this->item_provider){
		return;
	}

	unsigned longIndex = this->get_long_index();

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
				auto w = this->item_provider->get_widget(this->posIndex);
				auto& lp = this->get_layout_params_as_const<layout_params>(*w);
				vector2 d = this->dims_for_widget(*w, lp);
				this->push_back(w); // this is just optimization, to avoid creating same widget twice
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
				auto w = this->item_provider->get_widget(this->posIndex);
				auto& lp = this->get_layout_params_as_const<layout_params>(*w);
				vector2 d = this->dims_for_widget(*w, lp);
				this->insert(w, this->children().begin()); // this is just optimization, to avoid creating same widget twice
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

morda::vector2 list_widget::measure(const morda::vector2& quotum)const{
	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	vector2 ret(quotum);

	using std::max;
	ret[longIndex] = max(ret[longIndex], real(0)); // clamp bottom

	if(ret[transIndex] > 0){
		return ret;
	}

	ret[transIndex] = 0;

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		ret[transIndex] = max(ret[transIndex], (*i)->rect().d[transIndex]); // clamp bottom
	}

	return ret;
}

void list_widget::provider::notify_data_set_change(){
	if(!this->get_list()){
		return;
	}

	this->get_list()->context->run_from_ui_thread(
		[this](){
			this->get_list()->handleDataSetChanged();
		}
	);
}

void list_widget::handleDataSetChanged(){
	this->numTailItems = 0; // 0 means that it needs to be recomputed

	this->clear();
	this->addedIndex = size_t(-1);

	this->updateChildrenList();

	if(this->data_set_change_handler){
		this->data_set_change_handler(*this);
	}
}
