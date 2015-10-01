/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>
#include <list>

#include <utki/Unique.hpp>

#include "../../Exc.hpp"
#include "../Widget.hpp"


namespace morda{



class Container : virtual public Widget{

private:
	T_ChildrenList children_var;
	
	//Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	typedef std::map<unsigned, std::pair<std::weak_ptr<Widget>, unsigned> > T_MouseCaptureMap;
	T_MouseCaptureMap mouseCaptureMap;
	
	//flag indicating that modifications to children list are blocked
	bool isBlocked = false;
	
	
protected:
	virtual std::unique_ptr<LayoutParams> createLayoutParams(const stob::Node* chain = nullptr)const{
		return utki::makeUnique<Widget::LayoutParams>(chain);
	}
	
	virtual LayoutParams& getLayoutParams_internal(Widget& w);
	
	const LayoutParams& getLayoutParams_internal(const Widget& w)const{
		return const_cast<std::remove_const<std::remove_pointer<decltype(this)>::type>::type*>(this)->getLayoutParams_internal(
				const_cast<std::remove_const<std::remove_reference<decltype(w)>::type>::type&>(w)
			);
	}
	
	template <class T> const T& getLayoutParamsAs(const Widget& w)const{
		auto p = dynamic_cast<const T*>(&this->getLayoutParams_internal(w));
		if(!p){
			w.layoutParams.reset();
			p = dynamic_cast<const T*>(&this->getLayoutParams_internal(w));
		}
		
		ASSERT(p)
		return *p;
	}
	
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const;
	
public:
	LayoutParams& getLayoutParams(Widget& w){
		if(w.parent() != this){
			throw Exc("Container::getLayoutParams(): the widget is not added to this container");
		}		
		this->setRelayoutNeeded();
		return this->getLayoutParams_internal(w);
	}
	
	
public:
	Container(const stob::Node* chain = nullptr);

	~Container()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID) override;
	
	void onHoverChanged(unsigned pointerID)override;
	
	void layOut()override;
	
	T_ChildrenList::iterator add(const std::shared_ptr<Widget>& w, T_ChildrenList::const_iterator insertBefore);
	
	T_ChildrenList::iterator add(const std::shared_ptr<Widget>& w, const Widget* insertBefore = nullptr);

	void add(const stob::Node& chain);
	
	std::shared_ptr<Widget> remove(T_ChildrenList::const_iterator iter);
	
	std::shared_ptr<Widget> remove(Widget& w);
	
	void removeAll();
	
	std::shared_ptr<Widget> findChildByName(const std::string& name)noexcept override;
	
	const T_ChildrenList& children()const noexcept{
		return this->children_var;
	}
	
	/**
	 * @brief Called when children list changes.
	 * Default implementation requests re-layout.
     */
	virtual void onChildrenListChanged(){
		this->setRelayoutNeeded();
	}
};




template <class T> T* Widget::findParent(const char* name){
	if(!this->parent()){
		return nullptr;
	}

	auto p = dynamic_cast<T*>(this->parent());
	if(p && (!name || p->name() == name)){
		return p;
	}

	return this->parent()->findParent<T>();
}



}//~namespace
