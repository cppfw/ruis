/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>
#include <list>

#include <utki/Unique.hpp>

#include "../../../Exc.hpp"
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
	
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const;
	
protected:
	virtual LayoutParams& getLayoutParamsDuringLayout(Widget& w);
	
	const LayoutParams& getLayoutParamsDuringLayout(const Widget& w)const{
		return const_cast<std::remove_const<std::remove_pointer<decltype(this)>::type>::type*>(this)->getLayoutParamsDuringLayout(
				const_cast<std::remove_const<std::remove_reference<decltype(w)>::type>::type&>(w)
			);
	}
	
	template <class T> const T& getLayoutParamsDuringLayoutAs(const Widget& w)const{
		auto p = dynamic_cast<const T*>(&this->getLayoutParamsDuringLayout(w));
		if(!p){
			w.layoutParams.reset();
			p = dynamic_cast<const T*>(&this->getLayoutParamsDuringLayout(w));
		}
		
		ASSERT(p)
		return *p;
	}
	
public:
	LayoutParams& getLayoutParams(Widget& w);
	
	const LayoutParams& getLayoutParams(Widget& w)const;
	
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




template <class T> T* Widget::findAncestor(const char* name){
	if(!this->parent()){
		return nullptr;
	}
	
	auto p = dynamic_cast<T*>(this->parent());
	
	if(p){
		if(!name || p->name() == name){
			return p;
		}
	}

	return this->parent()->findAncestor<T>();
}



}//~namespace
