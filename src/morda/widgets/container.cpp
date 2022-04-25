/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "container.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

using namespace morda;

container::container(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
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

	if(!w.layout_params_){
		w.layout_params_ = this->create_layout_params(w.layout_desc);
	}

	return *w.layout_params_;
}

std::unique_ptr<widget::layout_params> container::create_layout_params(const treeml::forest& desc)const{
	return std::make_unique<widget::layout_params>(desc, this->context->units);
}

void container::push_back_inflate(const treeml::forest& desc){
	for(auto i = desc.begin(); i != desc.end(); ++i){
		if(is_leaf_property(i->value)){
			continue;
		}
		this->push_back(this->context->inflater.inflate(i, i + 1));
	}

	// in case this widget is initially disabled, as specified in gui script
	// we need to update enabled state of children
	if(!this->is_enabled()){
		this->container::on_enable_change();
	}
}

void container::render_child(const matrix4& matrix, const widget& c) const {
	if(!c.is_visible()){
		return;
	}

	morda::matrix4 matr(matrix);
	matr.translate(c.rect().p);

	c.render_internal(matr);
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
		auto i = this->mouse_capture_map.find(e.pointer_id);
		if(i != this->mouse_capture_map.end()){
			if(auto w = i->second.capturing_widget.lock()){
				if(w->is_interactive()){
					w->on_mouse_button(mouse_button_event{
							e.is_down,
							e.pos - w->rect().p,
							e.button,
							e.pointer_id
						});
					w->set_hovered(w->rect().overlaps(e.pos), e.pointer_id);

					unsigned& num_buttons_captured = i->second.num_buttons_captured;
					if(e.is_down){
						// if we get button down event for mouse capturing widget, then it is not for the buttons
						// which are already down, so we increase the button counter.
						++num_buttons_captured;
					}else{
						//If we get button up event,
						// then it is for one of the buttons which was down before, so we decrease the button counter.
						--num_buttons_captured;
					}
					if(num_buttons_captured == 0){
						this->mouse_capture_map.erase(i);
					}

					// doesn't matter what to return because parent widget also captured
					// the mouse and in this case the return value is ignored
					return true;
				}
			}
			this->mouse_capture_map.erase(i);
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
			ASSERT(this->mouse_capture_map.find(e.pointer_id) == this->mouse_capture_map.end())

			// normally, we get here only when the mouse was not captured by widget, because
			// as soon as mouse button down event comes to some widget, it captures the mouse.
			// So, the event should be button down.
			// But, in theory, it can be button up event here, if some widget which captured
			// mouse was removed from its parent. So, check if we have button down event.
			if(e.is_down){
				this->mouse_capture_map.insert(std::make_pair(
						e.pointer_id,
						mouse_capture_info{
							utki::make_weak(c),
							1
						}
					));
			}

			// widget has consumed the mouse button event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those
			for(++i; i != this->children().rend(); ++i){
				(*i)->set_hovered(false, e.pointer_id);
			}

			return true;
		}
	}

	return this->widget::on_mouse_button(e);
}

bool container::on_mouse_move(const mouse_move_event& e){
//	TRACE(<< "container::on_mouse_move(): pos = " << pos << std::endl)

	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	if(!e.ignore_mouse_capture){
		auto i = this->mouse_capture_map.find(e.pointer_id);
		if(i != this->mouse_capture_map.end()){
			if(auto w = i->second.capturing_widget.lock()){
				if(w->is_interactive()){
					w->on_mouse_move(mouse_move_event{
							e.pos - w->rect().p,
							e.pointer_id,
							e.ignore_mouse_capture
						});
					w->set_hovered(w->rect().overlaps(e.pos), e.pointer_id);

					// doesn't matter what to return because parent widget also captured
					// the mouse and in this case the return value is ignored
					return true;
				}
			}
			this->mouse_capture_map.erase(i);
		}
	}

	// call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->is_interactive()){
			ASSERT_INFO(!c->is_hovered(), "c->name() = " << c->id)
			continue;
		}
		
		if(!c->rect().overlaps(e.pos)){
			c->set_hovered(false, e.pointer_id);
			continue;
		}

		c->set_hovered(true, e.pointer_id);

		// LOG("e.pos = " << e.pos << ", rect() = " << c->rect() << std::endl)
		if(c->on_mouse_move(mouse_move_event{
				e.pos - c->rect().p,
				e.pointer_id,
				e.ignore_mouse_capture
			}))
		{
			// widget has consumed the mouse move event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those
			for(++i; i != this->children().rend(); ++i){
				(*i)->set_hovered(false, e.pointer_id);
			}

			return true;
		}
	}

	return false;
}

void container::on_hover_change(unsigned pointer_id){
	if(this->is_hovered(pointer_id)){
		return;
	}

	// un-hover all the children since container became un-hovered
	blocked_flag_guard blocked_guard(this->is_blocked);
	for(auto& w : this->children()){
		w->set_hovered(false, pointer_id);
	}
}

void container::lay_out(){
//	TRACE(<< "container::lay_out(): invoked" << std::endl)
	for(auto& w : this->children()){
		if(w->is_layout_dirty()){
			w->layout_dirty = false;
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

	auto ret = this->children_list.variable.emplace(before, std::move(w));

	ww.parent_container = this;
	ww.on_parent_change();

	this->on_children_change();

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

	auto ret = this->children_list.variable.erase(child);

	w->parent_container = nullptr;
	w->set_unhovered();
	w->on_parent_change();

	this->on_children_change();

	return ret;
}

void container::clear(){
	for(auto i = this->children().begin(); i != this->children().end(); i = this->erase(i)){}
}

std::shared_ptr<widget> container::try_get_widget(const std::string& id, bool allow_itself)noexcept{
	if(allow_itself){
		if(auto r = this->widget::try_get_widget(id, true)){
			return r;
		}
	}

	for(auto& w : this->children()){
		if(auto r = w->try_get_widget(id, true)){
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
	if(!d.is_positive_or_zero()){
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

	auto b = this->children_list.variable.erase(before, before); // remove constness
	auto i = this->children_list.variable.erase(child, child); // remove constness

	decltype(child) ret;

	auto distance = std::distance(i, b);
	ASSERT(distance != 0)
	if(distance > 0){
		ret = std::rotate(i, std::next(i), b);
	}else{
		ret = std::rotate(b, i, std::next(i));
		--ret;
	}

	this->on_children_change();

	return ret;
}

container::widget_list::const_iterator container::find(const widget& w){
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[&w](const decltype(this->children_list.variable)::value_type& v) -> bool{
				return v.get() == &w;
			}
		);
}

container::const_widget_list::const_iterator container::find(const widget& w)const{
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[&w](const decltype(this->children_list.constant)::value_type& v) -> bool{
				return v.get() == &w;
			}
		);
}

void container::on_enable_change(){
	blocked_flag_guard blocked_guard(this->is_blocked);
	
	for(auto& c : this->children()){
		c->set_enabled(this->is_enabled());
	}
}
