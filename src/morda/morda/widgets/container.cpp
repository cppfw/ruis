#include "container.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

using namespace morda;

container::container(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{
	this->push_back_inflate(desc);
}

widget::layout_params& container::get_layout_params(widget& w){
	this->invalidate_layout();

	auto& lp = const_cast<
			std::add_pointer<
					std::add_const<
							std::remove_pointer<decltype(this)>::type
						>::type
				>::type
		>(this)->get_layout_params_const(w);

	return const_cast<
			std::add_lvalue_reference<
					std::remove_const<
							std::remove_reference<decltype(lp)>::type
						>::type
				>::type>(lp);
}

const widget::layout_params& container::get_layout_params_const(const widget& w)const{
	if(w.parent() && w.parent() != this){
		throw std::invalid_argument("container::get_layout_params(): the given widget is not a child of this container");
	}

	if(!w.layoutParams){
		w.layoutParams = this->create_layout_params(w.layout_desc);
	}

	return *w.layoutParams;
}

std::unique_ptr<widget::layout_params> container::create_layout_params(const puu::forest& desc)const{
	return utki::make_unique<widget::layout_params>(desc, this->context->units);
}

void container::push_back_inflate(const puu::forest& desc){
	for(auto i = desc.begin(); i != desc.end(); ++i){
		if(is_leaf_property(i->value)){
			continue;
		}
		this->push_back(this->context->inflater.inflate(i, i + 1));
	}
}

void container::render_child(const matrix4& matrix, const widget& c) const {
	if(!c.is_visible()){
		return;
	}

	morda::matrix4 matr(matrix);
	matr.translate(c.rect().p);

	c.renderInternal(matr);
}

void container::render(const morda::matrix4& matrix)const{
	for(auto& w: this->children()){
		this->render_child(matrix, *w);
	}
}

bool container::on_mouse_button(const mouse_button_event& e){
//	TRACE(<< "container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)

	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	{
		auto i = this->mouseCaptureMap.find(e.pointer_id);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
				if(w->is_interactive()){
					w->set_hovered(w->rect().overlaps(e.pos), e.pointer_id);
					w->on_mouse_button(mouse_button_event{
							e.is_down,
							e.pos - w->rect().p,
							e.button,
							e.pointer_id
						});

					unsigned& n = i->second.second;
					// TODO: why is this counter needed?
					if(e.is_down){
						++n;
					}else{
						--n;
					}
					if(n == 0){
						this->mouseCaptureMap.erase(i);
					}
					return true; // doesn't matter what to return
				}
			}
			this->mouseCaptureMap.erase(i);
		}
	}

	// call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->is_interactive()){
			continue;
		}

		if(!c->rect().overlaps(e.pos)){
			continue;
		}

		// Sometimes mouse click event comes without prior mouse move,
		// but, since we get mouse click, then the widget was hovered before the click.
		c->set_hovered(true, e.pointer_id);
		if(c->on_mouse_button(mouse_button_event{
				e.is_down,
				e.pos - c->rect().p,
				e.button,
				e.pointer_id
			}))
		{
			ASSERT(this->mouseCaptureMap.find(e.pointer_id) == this->mouseCaptureMap.end())

			if(e.is_down){ // in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(e.pointer_id, std::make_pair(std::weak_ptr<widget>(c), 1)));
			}

			return true;
		}
	}

	return this->widget::on_mouse_button(e);
}

bool container::on_mouse_move(const mouse_move_event& e){
//	TRACE(<< "container::OnMouseMove(): pos = " << pos << std::endl)

	blocked_flag_guard blocked_guard(this->is_blocked);

	// call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->is_interactive()){
			ASSERT_INFO(!c->is_hovered(), "c->name() = " << c->id)
			continue;
		}

		bool consumed = c->on_mouse_move(mouse_move_event{
				e.pos - c->rect().p,
				e.pointer_id
			});

		// set hovered goes after move notification because position of widget could change
		// during handling the notification, so need to check after that for hovering
		if(!c->rect().overlaps(e.pos)){
			c->set_hovered(false, e.pointer_id);
			continue;
		}

		c->set_hovered(true, e.pointer_id);

		if(consumed){ // consumed mouse move event
			// un-hover rest of the children
			for(++i; i != this->children().rend(); ++i){
				auto& c = *i;
				c->set_hovered(false, e.pointer_id);
			}
			return true;
		}
	}

	return this->widget::on_mouse_move(mouse_move_event{
			e.pos,
			e.pointer_id
		});
}

void container::on_hover_changed(unsigned pointerID){
	if(this->is_hovered(pointerID)){
		return;
	}

	// un-hover all the children if container became un-hovered
	blocked_flag_guard blocked_guard(this->is_blocked);
	for(auto& w : this->children()){
		w->set_hovered(false, pointerID);
	}
}

void container::lay_out(){
//	TRACE(<< "container::lay_out(): invoked" << std::endl)
	for(auto& w : this->children()){
		if(w->is_layout_invalid()){
			w->relayoutNeeded = false;
			w->lay_out();
		}
	}
}

container::widget_list::const_iterator container::insert(std::shared_ptr<widget> w, widget_list::const_iterator before){
	if(!w){
		throw std::invalid_argument("container::insert(): pointer to widget is a null pointer");
	}

	if(w->parent()){
		throw std::invalid_argument("container::insert(): given widget is already added to some container");
	}

	if(this->is_blocked){
		throw std::logic_error("container::insert(): children list is locked");
	}

	if(before != this->children().end() && (*before)->parent() != this){
		throw std::invalid_argument("container::insert(): given 'before' iterator points to a different container");
	}

	widget& ww = *w;

	auto ret = this->children_v.variable.emplace(before, std::move(w));

	ww.parent_v = this;
	ww.on_parent_changed();

	this->on_children_changed();

	ASSERT(!ww.is_hovered())
	return ret;
}

container::widget_list::const_iterator container::erase(widget_list::const_iterator child){
	if(this->is_blocked){
		throw std::logic_error("container::erase(): children list is locked");
	}

	if(child == this->children().end()){
		throw std::invalid_argument("container::erase(): given 'child' iterator is invalid");
	}

	if((*child)->parent() != this){
		throw std::invalid_argument("container::erase(): given child widget belongs to a different container");
	}

	auto w = *child;

	auto ret = this->children_v.variable.erase(child);

	w->parent_v = nullptr;
	w->set_unhovered();
	w->on_parent_changed();

	this->on_children_changed();

	return ret;
}

void container::clear(){
	for(auto i = this->children().begin(); i != this->children().end(); i = this->erase(i)){}
}

std::shared_ptr<widget> container::try_get_widget(const std::string& id)noexcept{
	if(auto r = this->widget::try_get_widget(id)){
		return r;
	}

	// first check direct children, because the closer to the tree root higher the priority is
	for(auto& w : this->children()){
		if(auto r = w->widget::try_get_widget(id)){
			return r;
		}
	}

	// then check deeper by tree
	for(auto& w : this->children()){
		if(auto r = w->try_get_widget(id)){
			return r;
		}
	}
	return nullptr;
}

vector2 container::dims_for_widget(const widget& w, const layout_params& lp)const{
	vector2 d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dims[i] == layout_params::max || lp.dims[i] == layout_params::fill){
			d[i] = this->rect().d[i];
		}else if(lp.dims[i] == layout_params::min){
			d[i] = -1; // will be updated below
		}else{
			d[i] = lp.dims[i];
		}
	}
	if(d.x < 0 || d.y < 0){
		vector2 md = w.measure(d);
		for(unsigned i = 0; i != md.size(); ++i){
			if(d[i] < 0){
				d[i] = md[i];
			}
		}
	}
	return d;
}

container::widget_list::const_iterator container::change_child_z_position(widget_list::const_iterator child, widget_list::const_iterator before){
	if(this->is_blocked){
		throw std::logic_error("container::change_child_z_position(): children list is locked");
	}

	if(child == this->children().end()){
		throw std::invalid_argument("container::change_child_z_position(): given 'child' iterator is invalid");
	}

	if((*child)->parent() != this){
		throw std::invalid_argument("container::change_child_z_position(): given child widget belongs to a different container");
	}

	if(before != this->children().end() && (*before)->parent() != this){
		throw std::invalid_argument("container::change_child_z_position(): given 'before' iterator points to a different container");
	}

	if(child == before){
		// child is already at the right place
		return child;
	}

	auto b = this->children_v.variable.erase(before, before); // remove constness
	auto i = this->children_v.variable.erase(child, child); // remove constness

	decltype(child) ret;

	auto distance = std::distance(i, b);
	ASSERT(distance != 0)
	if(distance > 0){
		ret = std::rotate(i, std::next(i), b);
	}else{
		ret = std::rotate(b, i, std::next(i));
		--ret;
	}

	this->on_children_changed();

	return ret;
}

container::widget_list::const_iterator container::find(const widget& w){
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[&w](const decltype(this->children_v.variable)::value_type& v) -> bool{
				return v.get() == &w;
			}
		);
}

container::const_widget_list::const_iterator container::find(const widget& w)const{
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[&w](const decltype(this->children_v.constant)::value_type& v) -> bool{
				return v.get() == &w;
			}
		);
}
