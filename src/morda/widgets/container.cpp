#include "container.hpp"

#include "../exception.hpp"
#include "../Morda.hpp"

#include "../util/util.hpp"



using namespace morda;



container::container(const puu::trees& desc) :
		widget(desc)
{
	this->inflate_push_back(desc);
}

widget::layout_params& container::get_layout_params(widget& w){
	this->invalidate_layout();

	auto& lp = const_cast<
			std::add_pointer<
					std::add_const<
							std::remove_pointer<decltype(this)>::type
						>::type
				>::type
		>(this)->get_layout_params(w);

	return const_cast<
			std::add_lvalue_reference<
					std::remove_const<
							std::remove_reference<decltype(lp)>::type
						>::type
				>::type>(lp);
}

const widget::layout_params& container::get_layout_params(const widget& w)const{
	if(w.parent() && w.parent() != this){
		throw std::invalid_argument("container::get_layout_params(): the given widget is not a child of this container");
	}

	if(!w.layoutParams){
		w.layoutParams = this->create_layout_params(w.layout_desc);
	}

	return *w.layoutParams;
}

void container::inflate_push_back(const puu::trees& desc){
	for(auto i = desc.begin(); i != desc.end(); ++i){
		if(is_leaf_property(i->value)){
			continue;
		}
		this->push_back(morda::inst().inflater.inflate(i, i + 1));
	}
}

void container::render_child(const Matr4r& matrix, const Widget& c) const {
	if(!c.is_visible()){
		return;
	}

	morda::Matr4r matr(matrix);
	matr.translate(c.rect().p);

	c.renderInternal(matr);
}


void container::render(const morda::Matr4r& matrix)const{
	for(auto& w: this->children()){
		this->render_child(matrix, *w);
	}
}



bool container::on_mouse_button(bool isDown, const vector2& pos, mouse_button button, unsigned pointerId){
//	TRACE(<< "container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)

	BlockedFlagGuard blockedFlagGuard(this->isBlocked);

	//check if mouse captured
	{
		auto i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
				if(w->is_interactive()){
					w->set_hovered(w->rect().overlaps(pos), pointerId);
					w->on_mouse_button(isDown, pos - w->rect().p, button, pointerId);

					unsigned& n = i->second.second;
					//TODO: why is this counter needed?
					if(isDown){
						++n;
					}else{
						--n;
					}
					if(n == 0){
						this->mouseCaptureMap.erase(i);
					}
					return true;//doesn't matter what to return
				}
			}
			this->mouseCaptureMap.erase(i);
		}
	}

	//call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->is_interactive()){
			continue;
		}

		if(!c->rect().overlaps(pos)){
			continue;
		}

		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget was hovered before the click.
		c->set_hovered(true, pointerId);
		if(c->on_mouse_button(isDown, pos - c->rect().p, button, pointerId)){
			ASSERT(this->mouseCaptureMap.find(pointerId) == this->mouseCaptureMap.end())

			if(isDown){//in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(pointerId, std::make_pair(std::weak_ptr<Widget>(c), 1)));
			}

			return true;
		}
	}

	return this->widget::on_mouse_button(isDown, pos, button, pointerId);
}



bool container::on_mouse_move(const vector2& pos, unsigned pointerID){
//	TRACE(<< "container::OnMouseMove(): pos = " << pos << std::endl)

	BlockedFlagGuard blockedFlagGuard(this->isBlocked);

	//call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->is_interactive()){
			ASSERT_INFO(!c->is_hovered(), "c->name() = " << c->name())
			continue;
		}

		bool consumed = c->on_mouse_move(pos - c->rect().p, pointerID);

		//set hovered goes after move notification because position of widget could change
		//during handling the notification, so need to check after that for hovering
		if(!c->rect().overlaps(pos)){
			c->set_hovered(false, pointerID);
			continue;
		}

		c->set_hovered(true, pointerID);

		if(consumed){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != this->children().rend(); ++i){
				auto& c = *i;
				c->set_hovered(false, pointerID);
			}
			return true;
		}
	}

	return this->Widget::on_mouse_move(pos, pointerID);
}



void container::on_hover_changed(unsigned pointerID){
	if(this->is_hovered(pointerID)){
		return;
	}

	//un-hover all the children if container became un-hovered
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
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



container::list::const_iterator container::insert(std::shared_ptr<Widget> w, list::const_iterator before){
	if(!w){
		throw std::invalid_argument("container::insert(): pointer to widget is a null pointer");
	}

	if(w->parent()){
		throw std::invalid_argument("container::insert(): given widget is already added to some container");
	}

	if(this->isBlocked){
		throw morda::exception("container::insert(): children list is locked");
	}

	if(before != this->children().end() && (*before)->parent() != this){
		throw std::invalid_argument("container::insert(): given 'before' iterator points to a different container");
	}

	Widget& ww = *w;

	auto ret = this->children_v.variable.emplace(before, std::move(w));

	ww.parent_v = this;
	ww.on_parent_changed();

	this->on_children_changed();

	ASSERT(!ww.is_hovered())
	return ret;
}

std::shared_ptr<Widget> container::remove(list::const_iterator iter){
	return this->remove(**iter);
}


std::shared_ptr<Widget> container::remove(Widget& w){
	auto ret = w.sharedFromThis(&w);

	this->erase(this->find(&w));

	return ret;
}

container::list::const_iterator container::erase(list::const_iterator child){
	if(this->isBlocked){
		throw morda::exception("container::erase(): children list is locked");
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

void container::clear() {
	for(auto i = this->children().begin(); i != this->children().end(); i = this->erase(i)){}
}



std::shared_ptr<Widget> container::try_get_widget(const std::string& id)noexcept{
	if(auto r = this->Widget::try_get_widget(id)){
		return r;
	}

	//first check direct children, because the closer to the tree root higher the priority is
	for(auto& w : this->children()){
		if(auto r = w->Widget::try_get_widget(id)){
			return r;
		}
	}

	//then check deeper by tree
	for(auto& w : this->children()){
		if(auto r = w->try_get_widget(id)){
			return r;
		}
	}
	return nullptr;
}



vector2 container::dims_for_widget(const widget& w, const layout_params& lp)const{
	Vec2r d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dim[i] == LayoutParams::max_c || lp.dim[i] == LayoutParams::fill_c){
			d[i] = this->rect().d[i];
		}else if(lp.dim[i] == LayoutParams::min_c){
			d[i] = -1; // will be updated below
		}else{
			d[i] = lp.dim[i];
		}
	}
	if(d.x < 0 || d.y < 0){
		Vec2r md = w.measure(d);
		for(unsigned i = 0; i != md.size(); ++i){
			if(d[i] < 0){
				d[i] = md[i];
			}
		}
	}
	return d;
}

container::list::const_iterator container::change_child_z_position(list::const_iterator child, list::const_iterator before) {
	if(this->isBlocked){
		throw morda::exception("container::change_child_z_position(): children list is locked");
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

container::list::const_iterator container::find(const Widget* w){
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[w](const decltype(this->children_v.variable)::value_type& v) -> bool{
				return v.get() == w;
			}
		);
}
