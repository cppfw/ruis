#pragma once

#include <map>
#include <list>

#include <utki/Unique.hpp>

#include "../../../Exc.hpp"
#include "../Widget.hpp"


namespace morda{



/**
 * @brief Container widget.
 * A widget which can contain children widgets.
 * From GUI script it can be instantiated as 'Container' and can  have child widgets specified. For example:
 * @code
 * Container{
 *     Text{
 *         text{"Hello world!"}
 *     }
 *     Text{
 *         text{"Hi!"}
 *     }
 * }
 * @endcode
 */
class Container : virtual public Widget{

private:
	T_ChildrenList children_v;
	
	//Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	typedef std::map<unsigned, std::pair<std::weak_ptr<Widget>, unsigned> > T_MouseCaptureMap;
	T_MouseCaptureMap mouseCaptureMap;
	
	//flag indicating that modifications to children list are blocked
	bool isBlocked = false;
	
	
protected:
	/**
	 * @brief Create layout parameters object specific for this container.
	 * Other widgets which subclass Container and need additional parameters for laying out children widgets should extend the Widget::LayoutParams class
	 * and override this method to construct the correct LayoutParams instance.
	 * @param chain - STOB chain describing layout parameters.
	 * @return A new instance of layout parameters object.
	 */
	virtual std::unique_ptr<LayoutParams> createLayoutParams(const stob::Node* chain = nullptr)const{
		return utki::makeUnique<Widget::LayoutParams>(chain);
	}
	
	/**
	 * @brief Calculate basic dimensions of widget.
	 * Calculates basic dimensions of given widget if it would be placed to
	 * this container with given layout parameters, basically this is just
	 * resolving of 'min', 'max' and 'fill' special values of dimensions.
	 * @param w - widget to calculate dimensions for.
	 * @param lp - layout parameters of the widget.
	 * @return Dimensions of widget.
	 */
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const;
	
protected:
	/**
	 * @brief Get layout parameters of child widget.
	 * This method is for getting layout parameters when laying out child widgets.
	 * Some subclass container can override this method to provide modified layout parameters while still use inherited layout algorithm.
	 * @param w - child widget to get layout parameters for.
	 * @return Layout parameters for given child widget.
	 */
	virtual const LayoutParams& getLayoutParamsDuringLayout(const Widget& w)const;
	
	/**
	 * @brief Get layout parameters of child widget.
	 * Same as getLayoutParamsDuringLayout() but also tries to cast the LayoutParams object to specified class.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
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
	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
	LayoutParams& getLayoutParams(Widget& w);
	
	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
	const LayoutParams& getLayoutParams(const Widget& w)const;
	
	using Widget::getLayoutParams;
public:
	/**
	 * @brief Constructor.
	 * @param chain - STOB chain describing the widget. Can be nullptr in which case empty container widget is created.
	 */
	Container(const stob::Node* chain = nullptr);
	
	/**
	 * @brief Render to screen.
	 * This is an override of Widget::render(). It just renders all container widgets.
	 * Normally, users do not need to call this method, it will be called by framework when needed.
	 */
	void render(const morda::Matr4r& matrix)const override;

	/**
	 * @brief Handle mouse button event.
	 * Override of Widget::onMouseButton() method. It passes the event to the container's child widgets in reverse order.
	 * Normally, users do not need to call this method, it will be called by framework when needed.
	 */
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) override;
	
	/**
	 * @brief Handle mouse move event.
	 * Override of Widget::onMouseMove() method. It passes the event to the container's child widgets in reverse order.
	 * Normally, users do not need to call this method, it will be called by framework when needed.
	 */
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID) override;
	
	/**
	 * @brief Handle hover changed event.
	 * Normally, users do not need to call this method, it will be called by framework when needed.
	 */
	void onHoverChanged(unsigned pointerID)override;
	
	/**
	 * @brief Layout child widgets.
	 * This implementation of layout method checks if any of child widgets needs re-layout and if so it calls layout method on them.
	 */
	void layOut()override;
	
	/**
	 * @brief Add child widget.
	 * @param w - widget to add.
	 * @param insertBefore - iterator into the list of children widgets before which to insert the new widget.
	 * @return Iterator for the newly added widget.
	 */
	T_ChildrenList::iterator add(std::shared_ptr<Widget> w, T_ChildrenList::const_iterator insertBefore);
	
	/**
	 * @brief Add child widget.
	 * @param w - widget to add.
	 * @param insertBefore - child widget before which to insert the new widget.
	 * @return Iterator of the newly added widget.
	 */
	T_ChildrenList::iterator add(std::shared_ptr<Widget> w, const Widget* insertBefore = nullptr);

	/**
	 * @brief Add child widgets inflating them from GUI description.
	 * @param chain - STOB chain describing child widgets to add.
	 */
	void add(const stob::Node& chain);
	
	/**
	 * @brief Remove child widget.
	 * @param iter - iterator of the widget to remove.
	 * @return Removed widget.
	 * @throw morda::Exc if iterator does not point to child widget of this container.
	 */
	std::shared_ptr<Widget> remove(T_ChildrenList::const_iterator iter);
	
	/**
	 * @brief Remove child widget.
	 * @param w - child widget to remove.
	 * @return Removed widget.
	 * @throw morda::Exc if given widget is not a child widget of this container.
	 */
	std::shared_ptr<Widget> remove(Widget& w);
	
	/**
	 * @brief Remove all child widgets.
	 */
	void removeAll();
	
	/**
	 * @brief Find child widget by name.
	 * It searches through the whole widget sub-hierarchy, not just direct children of this container.
	 * @param name - name of the child widget to look for.
	 * @return pointer to widget with given name if found.
	 * @return nullptr if no widget with given name found.
	 */
	std::shared_ptr<Widget> findChildByName(const std::string& name)noexcept override;
	
	/**
	 * @brief Get list of child widgets.
	 * @return List of child widgets.
	 */
	const T_ChildrenList& children()const noexcept{
		return this->children_v;
	}
	
	/**
	 * @brief Called when children list changes.
	 * This implementation requests re-layout.
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



}
