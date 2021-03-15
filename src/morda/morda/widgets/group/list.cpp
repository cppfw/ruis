#include "list.hpp"

#include "../../context.hpp"

using namespace morda;

namespace{
class static_provider : public list_widget::provider{
	std::vector<treeml::tree> widgets;
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


	void add(treeml::tree&& w){
		this->widgets.emplace_back(std::move(w));
	}
};
}

list_widget::list_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc, bool vertical):
		widget(std::move(c), desc),
		container(this->context, treeml::forest()),
		oriented_widget(this->context, treeml::forest(), vertical)
{
	std::shared_ptr<static_provider> pr = std::make_shared<static_provider>();

	for(const auto& p : desc){
		if(is_property(p)){
			continue;
		}

		pr->add(treeml::tree(p));
	}

	this->set_provider(std::move(pr));
}

void list_widget::lay_out(){
//	TRACE(<< "list_widget::lay_out(): invoked" << std::endl)

	this->num_tail_items = 0; // 0 means that it needs to be recomputed

	this->update_children_list();

	// defer the scroll position change notification, because layouting happens during render phase
	this->context->run_from_ui_thread(
			[wl = utki::make_weak_from(*this)](){
				if(auto l = wl.lock()){
					l->notify_scroll_pos_changed();
				}
			}
		);
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
	this->handle_data_set_changed();
}

real list_widget::calc_num_visible_items()const noexcept{
	// calculate number of visible items,
	// this number can have fraction part because of partially visible items
	real items_num = 0;

	unsigned index = this->get_long_index();

	for(auto& c : this->children()){
		auto dim = c->rect().d[index];
		auto visible_dim = dim;
		auto pos = c->rect().p[index];
		if(pos < 0){
			visible_dim += pos;
		}
		auto list_dim = this->rect().d[index];
		auto end_pos = pos + dim;
		if(end_pos > list_dim){
			visible_dim -= (end_pos - list_dim);
		}

		items_num += visible_dim / dim;
	}

	return items_num;
}

real list_widget::get_scroll_band()const noexcept{
	if(!this->item_provider || this->item_provider->count() == 0){
		return 0;
	}

	auto items_num = this->calc_num_visible_items();

	return items_num / morda::real(this->item_provider->count());
}

real list_widget::get_scroll_factor()const noexcept{
	if(!this->item_provider || this->item_provider->count() == 0){
		return 0;
	}

	if(this->children().size() == 0){
		return 0;
	}

	ASSERT(this->item_provider->count() >= this->num_tail_items)

	size_t length = this->item_provider->count() - this->num_tail_items;

	if(length == 0){
		return 0;
	}

	if(this->num_tail_items == 0){
		return 0;
	}

	auto index = this->get_long_index();

	real list_dim = this->rect().d[index];

	// calculate average item dimension from visible items
	auto average_item_dim = list_dim / this->calc_num_visible_items();

	if(average_item_dim <= 0){
		return 0;
	}

	return (real(this->pos_index + this->pos_offset / this->children().front()->rect().d[index]) * average_item_dim)
			/ (real(length + this->first_tail_item_offset / this->first_tail_item_dim) * average_item_dim);
}

void list_widget::set_scroll_factor(real factor){
	if(!this->item_provider || this->item_provider->count() == 0){
		return;
	}

	if(this->num_tail_items == 0){
		this->update_tail_items_info();
	}

	size_t old_index = this->pos_index;
	real old_offset = this->pos_offset;

	this->pos_index = size_t(factor * real(this->item_provider->count() - this->num_tail_items));

//	TRACE(<< "list_widget::setScrollPosAsFactor(): this->pos_index = " << this->pos_index << std::endl)

	if(this->item_provider->count() != this->num_tail_items){
		real intFactor = real(this->pos_index) / real(this->item_provider->count() - this->num_tail_items);

		if(this->children().size() != 0){
			real d = this->rect().d[this->get_long_index()];
			d = (d + this->first_tail_item_offset) / this->num_tail_items;

			this->pos_offset = ::round(d * (factor - intFactor) * real(this->item_provider->count() - this->num_tail_items) + factor * this->first_tail_item_offset);
		}else{
			this->pos_offset = 0;
		}
	}else{
		ASSERT(this->pos_index == 0)
		this->pos_offset = ::round(factor * this->first_tail_item_offset);
	}

	this->update_children_list();

	this->notify_scroll_pos_changed(old_index, old_offset);
}

// TODO: refactor
bool list_widget::arrange_widget(std::shared_ptr<widget>& w, real& pos, bool added, size_t index, widget_list::const_iterator& insertBefore){
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
		if(this->added_index > index){
			this->added_index = index;
		}
	}else{
		++this->pos_index;
		this->pos_offset -= w->rect().d[longIndex];
		if(added){
			insertBefore = this->erase(insertBefore);
			if(this->item_provider){
				this->item_provider->recycle(index, w);
			}
			++this->added_index;
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
void list_widget::update_children_list(){
	if(!this->item_provider){
		this->pos_index = 0;
		this->pos_offset = 0;

		this->clear();
		this->added_index = size_t(-1);
		return;
	}

	if(this->num_tail_items == 0){
		this->update_tail_items_info();
	}

	if(this->pos_index == this->first_tail_item_index){
		if(this->pos_offset > this->first_tail_item_offset){
			this->pos_offset = this->first_tail_item_offset;
		}
	}else if(this->pos_index > this->first_tail_item_index){
		this->pos_index = this->first_tail_item_index;
		this->pos_offset = this->first_tail_item_offset;
	}

	real pos = -this->pos_offset;

//	TRACE(<< "list_widget::update_children_list(): this->added_index = " << this->added_index << " this->pos_index = " << this->pos_index << std::endl)

	// remove widgets from top
	for(; this->children().size() != 0 && this->added_index < this->pos_index; ++this->added_index){
		auto i = this->children().begin();
		auto w = *i;
		this->erase(i);
		if(this->item_provider){
			this->item_provider->recycle(this->added_index, w);
		}
	}

	auto iter = this->children().begin();
	size_t iterIndex = this->added_index;
	const size_t iterEndIndex = iterIndex + this->children().size();
	size_t index = this->pos_index;
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

		is_last = this->arrange_widget(w, pos, isAdded, index, iter);
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

void list_widget::update_tail_items_info(){
	this->num_tail_items = 0;

	if(!this->item_provider || this->item_provider->count() == 0){
		return;
	}

	unsigned longIndex = this->get_long_index();

	real dim = this->rect().d[longIndex];

	ASSERT(this->item_provider)
	ASSERT(this->item_provider->count() > 0)

	for(size_t i = this->item_provider->count(); i != 0 && dim > 0; --i){
		++this->num_tail_items;

		auto w = this->item_provider->get_widget(i - 1);
		ASSERT(w)

		auto& lp = this->get_layout_params_as_const<layout_params>(*w);

		vector2 d = this->dims_for_widget(*w, lp);

		auto item_dim = d[longIndex];
		dim -= item_dim;
		this->first_tail_item_dim = item_dim;
	}

	this->first_tail_item_index = this->item_provider->count() - this->num_tail_items;

	if(dim > 0){
		this->first_tail_item_offset = -1;
	}else{
		this->first_tail_item_offset = -dim;
	}

	if(this->first_tail_item_dim <= 0){
		this->first_tail_item_dim = 1;
	}
}

void list_widget::notify_scroll_pos_changed(){
	if(this->scroll_change_handler){
		this->scroll_change_handler(*this);
	}
}

void list_widget::notify_scroll_pos_changed(size_t old_index, real old_offset){
	if(old_index != this->pos_index || old_offset != this->pos_offset){
		this->notify_scroll_pos_changed();
	}
}

void list_widget::scroll_by(real delta) {
	if(!this->item_provider){
		return;
	}

	size_t old_index = this->pos_index;
	real old_offset = this->pos_offset;

	unsigned longIndex = this->get_long_index();

//	TRACE(<< "delta = " << delta << std::endl)

	if(delta >= 0){
		for(auto& c : this->children()){
			auto wd = c->rect().d[longIndex] - this->pos_offset;
			if(wd > delta){
				this->pos_offset += delta;
				delta -= wd;
				break;
			}

			delta -= wd;
			this->pos_offset = 0;
			++this->pos_index;
		}

		if(delta > 0){
			ASSERT_INFO(
					this->pos_index > this->added_index + this->children().size(),
					"this->pos_index = " << this->pos_index
					<< " this->added_index = " << this->added_index
					<< " this->children().size() = " << this->children().size()
				)
			for(; this->pos_index < this->first_tail_item_index;){
				auto w = this->item_provider->get_widget(this->pos_index);
				auto& lp = this->get_layout_params_as_const<layout_params>(*w);
				vector2 d = this->dims_for_widget(*w, lp);
				this->push_back(w); // this is just optimization, to avoid creating same widget twice
				if(d[longIndex] > delta){
					this->pos_offset = delta;
					break;
				}
				delta -= d[longIndex];
				ASSERT(this->pos_offset == 0)
				++this->pos_index;
			}
		}
	}else{
		delta = -delta;
		if(delta <= this->pos_offset){
			this->pos_offset -= delta;
		}else{
			ASSERT(this->added_index == this->pos_index)
			delta -= this->pos_offset;
			this->pos_offset = 0;
			for(; this->pos_index > 0;){
				ASSERT(this->added_index == this->pos_index)
				--this->pos_index;
				auto w = this->item_provider->get_widget(this->pos_index);
				auto& lp = this->get_layout_params_as_const<layout_params>(*w);
				vector2 d = this->dims_for_widget(*w, lp);
				this->insert(w, this->children().begin()); // this is just optimization, to avoid creating same widget twice
				--this->added_index;
				if(d[longIndex] > delta){
					this->pos_offset = d[longIndex] - delta;
					break;
				}
				delta -= d[longIndex];
			}
		}
	}

	this->update_children_list();

	this->notify_scroll_pos_changed(old_index, old_offset);
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
			this->get_list()->handle_data_set_changed();
		}
	);
}

void list_widget::handle_data_set_changed(){
	this->num_tail_items = 0; // 0 means that it needs to be recomputed

	this->clear();
	this->added_index = size_t(-1);

	this->update_children_list();

	if(this->data_set_change_handler){
		this->data_set_change_handler(*this);
	}
}
