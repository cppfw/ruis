#include "Container.hpp"

#include "../exception.hpp"
#include "../Morda.hpp"

#include "../util/util.hpp"



using namespace morda;



Container::Container(const stob::Node* chain) :
		Widget(chain)
{
	if(chain){
		this->add(*chain);
	}
}

Widget::LayoutParams& Container::getLayoutParams(Widget& w){
	this->setRelayoutNeeded();

	auto& lp = const_cast<std::add_pointer<std::add_const<std::remove_pointer<decltype(this)>::type>::type>::type>(this)->getLayoutParams(w);

	return const_cast<std::add_lvalue_reference<std::remove_const<std::remove_reference<decltype(lp)>::type>::type>::type>(lp);
}

const Widget::LayoutParams& Container::getLayoutParams(const Widget& w)const{
	if(w.parent() && w.parent() != this){
		throw morda::Exc("Container::getLayoutParams(): the widget is added to another container");
	}

	if(!w.layoutParams){
		w.layoutParams = this->createLayoutParams(w.layout.get());
	}

	return *w.layoutParams;
}

void Container::add(const stob::Node& chain){
	for(auto n = chain.thisOrNextNonProperty().get_node(); n; n = n->nextNonProperty().get_node()){
		this->push_back(morda::Morda::inst().inflater.inflate(*n));
	}
}

void Container::renderChild(const Matr4r& matrix, const Widget& c) const {
	if(!c.isVisible()){
		return;
	}

	morda::Matr4r matr(matrix);
	matr.translate(c.rect().p);

	c.renderInternal(matr);
}


void Container::render(const morda::Matr4r& matrix)const{
	for(auto& w: this->children()){
		this->renderChild(matrix, *w);
	}
}



bool Container::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)

	BlockedFlagGuard blockedFlagGuard(this->isBlocked);

	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
				if(w->isInteractive()){
					w->setHovered(w->rect().overlaps(pos), pointerId);
					w->onMouseButton(isDown, pos - w->rect().p, button, pointerId);

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

		if(!c->isInteractive()){
			continue;
		}

		if(!c->rect().overlaps(pos)){
			continue;
		}

		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget was hovered before the click.
		c->setHovered(true, pointerId);
		if(c->onMouseButton(isDown, pos - c->rect().p, button, pointerId)){
			ASSERT(this->mouseCaptureMap.find(pointerId) == this->mouseCaptureMap.end())

			if(isDown){//in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(pointerId, std::make_pair(std::weak_ptr<Widget>(c), 1)));
			}

			return true;
		}
	}

	return this->Widget::onMouseButton(isDown, pos, button, pointerId);
}



bool Container::onMouseMove(const morda::Vec2r& pos, unsigned pointerID){
//	TRACE(<< "Container::OnMouseMove(): pos = " << pos << std::endl)

	BlockedFlagGuard blockedFlagGuard(this->isBlocked);

	//call children in reverse order
	for(auto i = this->children().rbegin(); i != this->children().rend(); ++i){
		auto& c = *i;

		if(!c->isInteractive()){
			ASSERT_INFO(!c->isHovered(), "c->name() = " << c->name())
			continue;
		}

		bool consumed = c->onMouseMove(pos - c->rect().p, pointerID);

		//set hovered goes after move notification because position of widget could change
		//during handling the notification, so need to check after that for hovering
		if(!c->rect().overlaps(pos)){
			c->setHovered(false, pointerID);
			continue;
		}

		c->setHovered(true, pointerID);

		if(consumed){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != this->children().rend(); ++i){
				auto& c = *i;
				c->setHovered(false, pointerID);
			}
			return true;
		}
	}

	return this->Widget::onMouseMove(pos, pointerID);
}



void Container::onHoverChanged(unsigned pointerID){
	if(this->isHovered(pointerID)){
		return;
	}

	//un-hover all the children if container became un-hovered
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	for(auto& w : this->children()){
		w->setHovered(false, pointerID);
	}
}



void Container::layOut(){
//	TRACE(<< "Container::layOut(): invoked" << std::endl)
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	for(auto& w : this->children()){
		if(w->needsRelayout()){
			w->relayoutNeeded = false;
			w->layOut();
		}
	}
}



Container::list::const_iterator Container::insert(std::shared_ptr<Widget> w, list::const_iterator before){
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

	auto ret = this->children_v.emplace(before, std::move(w));

	ww.parent_v = this;
	ww.onParentChanged();

	this->onChildrenListChanged();

	ASSERT(!ww.isHovered())
	return ret;
}

std::shared_ptr<Widget> Container::remove(list::const_iterator iter){
	return this->remove(**iter);
}


std::shared_ptr<Widget> Container::remove(Widget& w){
	auto ret = w.sharedFromThis(&w);

	this->erase(this->find(&w));

	return ret;
}

Container::list::const_iterator Container::erase(list::const_iterator child){
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

	auto ret = this->children_v.erase(child);

	w->parent_v = nullptr;
	w->setUnhovered();
	w->onParentChanged();

	this->onChildrenListChanged();

	return ret;
}

void Container::removeAll() {
	while(this->children().size()){
//		TRACE(<< "Container::removeAll(): w = " << (this->Children().front().get()) << std::endl)
		this->remove(*this->children().front());
	}
}



std::shared_ptr<Widget> Container::findById(const std::string& id)noexcept{
	if(auto r = this->Widget::findById(id)){
		return r;
	}

	//first check direct children, because the closer to the tree root higher the priority is
	for(auto& w : this->children()){
		if(auto r = w->Widget::findById(id)){
			return r;
		}
	}

	//then check deeper by tree
	for(auto& w : this->children()){
		if(auto r = w->findById(id)){
			return r;
		}
	}
	return nullptr;
}



Vec2r Container::dimForWidget(const Widget& w, const LayoutParams& lp)const{
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

Container::list::const_iterator Container::change_child_z_position(list::const_iterator child, list::const_iterator before) {
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

	auto b = this->children_v.erase(before, before); // remove constness
	auto i = this->children_v.erase(child, child); // remove constness

	decltype(child) ret;

	auto distance = std::distance(i, b);
	ASSERT(distance != 0)
	if(distance > 0){
		ret = std::rotate(i, std::next(i), b);
	}else{
		ret = std::rotate(b, i, std::next(i));
		--ret;
	}

	this->onChildrenListChanged();

	return ret;
}

Container::list::const_iterator Container::find(const Widget* w){
	return std::find_if(
			this->children().begin(),
			this->children().end(),
			[w](const decltype(this->children_v)::value_type& v) -> bool{
				return v.get() == w;
			}
		);
}
