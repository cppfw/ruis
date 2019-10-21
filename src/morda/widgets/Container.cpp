#include "Container.hpp"

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
		this->add(morda::Morda::inst().inflater.inflate(*n));
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
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->children().rbegin(); i != this->children().rend(); ++i){
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
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->children().rbegin(); i != this->children().rend(); ++i){
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


Widget::T_ChildrenList::iterator Container::add(std::shared_ptr<Widget> w, T_ChildrenList::const_iterator insertBefore){
	if(insertBefore == this->children().end()){
		return this->add(std::move(w));
	}
	return this->add(std::move(w), (*insertBefore).get());
}

Widget::T_ChildrenList::iterator Container::add(std::shared_ptr<Widget> w, const Widget* insertBefore){
	if(!w){
		return this->children_v.end();
	}

	ASSERT_INFO(w, "Container::Add(): widget pointer is 0")
	if(w->parent()){
		throw morda::Exc("Container::Add(): cannot add widget, it is already added to some container");
	}

	if(this->isBlocked){
		throw morda::Exc("Container::Add(): cannot add child while iterating through children, try deferred adding.");
	}

	if(insertBefore && insertBefore->parent() != this){
		throw morda::Exc("Container::Add(): cannot insert before provided iterator, it points to a different container");
	}

	T_ChildrenList::iterator ret;

	Widget& widget = *w;

	if(insertBefore){
		ret = this->children_v.insert(insertBefore->parentIter_v, std::move(w));
	}else{
		this->children_v.push_back(std::move(w));
		ret = this->children_v.end();
		--ret;
	}

	widget.parentIter_v = ret;
	widget.parent_v = this;
	widget.onParentChanged();

	this->onChildrenListChanged();

	ASSERT(!widget.isHovered())
	return ret;
}

std::shared_ptr<Widget> Container::remove(T_ChildrenList::const_iterator iter){
	return this->remove(**iter);
}


std::shared_ptr<Widget> Container::remove(Widget& w){
	if(w.parent_v != this){
		throw morda::Exc("Container::remove(): widget is not added to this container");
	}

	if(this->isBlocked){
		throw morda::Exc("Container::remove(): cannot remove child while iterating through children, try deferred removing.");
	}
//	TRACE(<< "Container::Remove(): w = " << (&w) << std::endl)

	ASSERT(w.parent_v == this)

	auto ret = *w.parentIter_v;

	this->children_v.erase(w.parentIter_v);

	w.parent_v = nullptr;
	w.setUnhovered();

	w.onParentChanged();

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

void Container::changeChildZPosition(Widget& child, T_ChildrenList::const_iterator toBefore) {
	if(child.parent() != this){
		throw morda::Exc("widget is not a child of this parent");
	}

	if(this->isBlocked){
		throw morda::Exc("cannot modify children list while iterating through children");
	}

	ASSERT(child.parent_v == this)

	auto w = *child.parentIter_v;

	this->children_v.erase(child.parentIter_v);

	child.parentIter_v = this->children_v.insert(toBefore, std::move(w));

	this->onChildrenListChanged();
}
