#include "Container.hpp"

#include "../../../App.hpp"

#include "../../../util/util.hpp"



using namespace morda;



namespace{
class BlockedFlagGuard{
	bool& blocked;
public:
	BlockedFlagGuard(bool& blocked) :
			blocked(blocked)
	{
		this->blocked = true;
	}
	
	~BlockedFlagGuard()noexcept{
		this->blocked = false;
	}
};
}



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



const Widget::LayoutParams& Container::getLayoutParamsDuringLayout(const Widget& w)const{
	return this->getLayoutParams(w);
}


void Container::add(const stob::Node& chain){
	for(auto n = chain.thisOrNextNonProperty().node(); n; n = n->nextNonProperty().node()){
		this->add(morda::App::inst().inflater.inflate(*n));
	}
}



//override
void Container::render(const morda::Matr4r& matrix)const{	
	for(auto& w: this->children()){
		if(!w->isVisible()){
			continue;
		}
		
		morda::Matr4r matr(matrix);
		matr.translate(w->rect().p);

		w->renderInternal(matr);
	}
}



bool Container::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID){
//	TRACE(<< "Container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerID);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
				w->setHovered(w->rect().overlaps(pos), pointerID);
				w->onMouseButton(isDown, pos - w->rect().p, button, pointerID);
				
				unsigned& n = i->second.second;
				if(isDown){
					++n;
				}else{
					--n;
				}
				if(n == 0){
					this->mouseCaptureMap.erase(i);
				}
				return true;//doesn't matter what to return
			}else{
				this->mouseCaptureMap.erase(i);
			}
		}
	}
	
	//call children in reverse order
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->children().rbegin(); i != this->children().rend(); ++i){
		if(!(*i)->isInteractive()){
			continue;
		}
		
		if(!(*i)->rect().overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget was hovered before the click.
		(*i)->setHovered(true, pointerID);
		if((*i)->onMouseButton(isDown, pos - (*i)->rect().p, button, pointerID)){
			ASSERT(this->mouseCaptureMap.find(pointerID) == this->mouseCaptureMap.end())
			
			if(isDown){//in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(pointerID, std::make_pair(std::weak_ptr<Widget>(*i), 1)));
			}
			
			return true;
		}
	}
	
	return this->Widget::onMouseButton(isDown, pos, button, pointerID);
}



bool Container::onMouseMove(const morda::Vec2r& pos, unsigned pointerID){
//	TRACE(<< "Container::OnMouseMove(): pos = " << pos << std::endl)
	
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerID);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
				w->onMouseMove(pos - w->rect().p, pointerID);
		
				//set hovered goes after move notification because position of widget could change
				//during handling the notification, so need to check after that for hovering
				w->setHovered(w->rect().overlaps(pos), pointerID);

				return true;//doesn't matter what to return
			}else{
				this->mouseCaptureMap.erase(i);
			}
		}
	}
	
	//call children in reverse order
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->children().rbegin(); i != this->children().rend(); ++i){
		if(!(*i)->isInteractive()){
			ASSERT(!(*i)->isHovered())
			continue;
		}
		
		if(!(*i)->rect().overlaps(pos)){
			(*i)->setHovered(false, pointerID);
			continue;
		}
		
		(*i)->setHovered(true, pointerID);
		
		if((*i)->onMouseMove(pos - (*i)->rect().p, pointerID)){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != this->children().rend(); ++i){
				(*i)->setHovered(false, pointerID);
			}
			return true;
		}		
	}
	
	return this->Widget::onMouseMove(pos, pointerID);
}



void Container::onHoverChanged(unsigned pointerID){
	//TODO: if some child removed during iterating?
	
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


Widget::T_ChildrenList::iterator Container::add(const std::shared_ptr<Widget>& w, T_ChildrenList::const_iterator insertBefore){
	if(insertBefore == this->children().end()){
		return this->add(w);
	}
	return this->add(w, (*insertBefore).get());
}

Widget::T_ChildrenList::iterator Container::add(const std::shared_ptr<Widget>& w, const Widget* insertBefore){
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
	
	if(insertBefore){
		ret = this->children_var.insert(insertBefore->parentIter, w);
	}else{
		this->children_var.push_back(w);
		ret = this->children_var.end();
		--ret;
	}
	
	w->parentIter = ret;
	w->parentContainer = this;
	w->onParentChanged();
	
	this->onChildrenListChanged();
	
	if(this->children_var.size() > 1){
		(*(++this->children_var.rbegin()))->onTopmostChanged();
	}
	
	w->onTopmostChanged();
	
	ASSERT(!w->isHovered())
	return ret;
}

std::shared_ptr<Widget> Container::remove(T_ChildrenList::const_iterator iter){
	return this->remove(**iter);
}


std::shared_ptr<Widget> Container::remove(Widget& w){
	if(w.parentContainer != this){
		throw morda::Exc("Container::Remove(): widget is not added to this container");
	}
	
	if(this->isBlocked){
		throw morda::Exc("Container::Remove(): cannot remove child while iterating through children, try deferred removing.");
	}
//	TRACE(<< "Container::Remove(): w = " << (&w) << std::endl)
	
	ASSERT(w.parentContainer == this)
	
	auto ret = *w.parentIter;
	
	this->children_var.erase(w.parentIter);
	
	w.parentContainer = nullptr;
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



std::shared_ptr<Widget> Container::findChildByName(const std::string& name)noexcept{
	if(auto r = this->Widget::findChildByName(name)){
		return r;
	}
	
	for(auto& w : this->children()){
		if(auto r = w->findChildByName(name)){
			return r;
		}
	}
	return nullptr;
}



Vec2r Container::dimForWidget(const Widget& w, const LayoutParams& lp)const{
	Vec2r d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dim[i] == LayoutParams::Max_d || lp.dim[i] == LayoutParams::Fill_d){
			d[i] = this->rect().d[i];
		}else if(lp.dim[i] == LayoutParams::Min_d){
			d[i] = -1; //will be updated later below
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
