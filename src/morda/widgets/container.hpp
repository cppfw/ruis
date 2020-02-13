#pragma once

#include <map>
#include <vector>

#include <utki/Unique.hpp>

#include "../util/util.hpp"
#include "widget.hpp"


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
class container : virtual public Widget{

public:
	typedef std::vector<std::shared_ptr<Widget>> list;
	typedef std::vector<std::shared_ptr<const Widget>> const_list;
private:
	static_assert(sizeof(list) == sizeof(const_list), "sizeof(list) differs from sizeof(const_list)");
	static_assert(sizeof(list::value_type) == sizeof(const_list::value_type), "sizeof(list::value_type) differs from sizeof(const_list::value_type)");

	// NOTE: according to C++11 standard it is undefined behaviour to read the inactive union member,
	//       but we rely on compiler implementing it the right way.
	union children_union{
		list variable;
		const_list constant; // this member never becomes active one, but we will read it when we need constant list of children

		children_union() :
				variable() // this sets the 'variable' member of the union as an active one
		{}
		~children_union(){}
	} children_v;

	// Map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	std::map<unsigned, std::pair<std::weak_ptr<Widget>, unsigned>> mouseCaptureMap;

private:
	// flag indicating that modifications to children list are blocked
	bool isBlocked = false;

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


protected:
	/**
	 * @brief Create layout parameters object specific for this container.
	 * Other widgets which subclass Container and need additional parameters for laying out children widgets should extend the Widget::LayoutParams class
	 * and override this method to construct the correct LayoutParams instance.
	 * @param desc - description of the layout parameters.
	 * @return A new instance of layout parameters object.
	 */
	virtual std::unique_ptr<LayoutParams> create_layout_params(const puu::forest& desc)const;

	/**
	 * @brief Calculate basic dimensions of widget.
	 * Calculates basic dimensions of given widget if it would be placed to
	 * this container with given layout parameters, basically this is just
	 * resolving of 'min', 'max' and 'fill' special values of dimensions.
	 * @param w - widget to calculate dimensions for.
	 * @param lp - layout parameters of the widget.
	 * @return Dimensions of widget.
	 */
	vector2 dims_for_widget(const widget& w, const layout_params& lp)const;

	//TODO: deprecated, remove.
	Vec2r dimForWidget(const Widget& w, const LayoutParams& lp)const{
		return this->dims_for_widget(w, lp);
	}

protected:

	/**
	 * @brief Get layout parameters of child widget.
	 * Same as get_layout_params() but also tries to cast the LayoutParams object to specified class.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
	template <class T> const T& get_layout_params_as(const widget& w)const{
		auto p = dynamic_cast<const T*>(&this->get_layout_params(w));
		if(!p){
			w.layoutParams.reset();
			p = dynamic_cast<const T*>(&this->get_layout_params(w));
		}

		ASSERT(p)
		return *p;
	}

	//TODO: deprecated, remove.
	template <class T> const T& getLayoutParamsAs(const Widget& w)const{
		return this->get_layout_params_as<T>(w);
	}

public:
	// struct layout_exception : public std::runtime_error{
	// 	layout_exception(const std::string& message) : std::runtime_error(message){}
	// };

	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 * @throw std::invalid_argument - in case the given widget is not a child of this container.
	 */
	layout_params& get_layout_params(widget& w);

	//TODO: deprecated, remove.
	LayoutParams& getLayoutParams(Widget& w){
		return this->get_layout_params(w);
	}

	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 * @throw std::invalid_argument - in case the given widget is not a child of this container.
	 */
	const layout_params& get_layout_params(const widget& w)const;

	//TODO: deprecated, remove.
	const LayoutParams& getLayoutParams(const Widget& w)const{
		return this->get_layout_params(w);
	}

	//TODO: deprecated, remove.
	using Widget::getLayoutParams;

	using widget::get_layout_params;

protected:

	void render_child(const matrix4& matrix, const widget& c)const;
public:
	/**
	 * @brief Constructor.
	 * @param c - context to which this widget belongs.
	 * @param desc - description of the widget.
	 */
	container(std::shared_ptr<morda::context> c, const puu::forest& desc);

	void render(const matrix4& matrix)const override;

	bool on_mouse_button(bool is_down, const vector2& pos, mouse_button button, unsigned pointer_id) override;

	bool on_mouse_move(const vector2& pos, unsigned pointer_id) override;

	void on_hover_changed(unsigned pointer_id)override;

	/**
	 * @brief Layout child widgets.
	 * This implementation of layout method checks if any of child widgets needs re-layout and if so it calls layout method on them.
	 */
	void lay_out()override;

	/**
	 * @brief Change Z order of a child widget.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param child - child widget to change Z order of.
	 * @param before - iterator into the children list before which to insert the child.
	 * @return new child iterator.
	 */
	list::const_iterator change_child_z_position(list::const_iterator child, list::const_iterator before);

	/**
	 * @brief Insert a widget to the container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param w - widget to insert.
	 * @param before - iterator within this container before which the widget will be inserted.
	 * @return iterator pointing to the newly inserted widget.
	 */
	list::const_iterator insert(std::shared_ptr<Widget> w, list::const_iterator before);

	/**
	 * @brief Insert a widget to the end of children list of the container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param w - widget to insert.
	 * @return iterator pointing to the newly inserted widget.
	 */
	list::const_iterator push_back(std::shared_ptr<Widget> w){
		return this->insert(std::move(w), this->children().end());
	}

	/**
	 * @brief Add child widgets inflating them from GUI description.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param desc - GUI description of the widgets to add.
	 */
	void inflate_push_back(const puu::forest& desc);

	//TODO: depreacted, remove.
	/**
	 * @brief Remove child widget.
	 * @param iter - iterator of the widget to remove.
	 * @return Removed widget.
	 * @throw morda::Exc if iterator does not point to child widget of this container.
	 */
	std::shared_ptr<Widget> remove(list::const_iterator iter);

	//TODO: depreacted, remove.
	/**
	 * @brief Remove child widget.
	 * @param w - child widget to remove.
	 * @return Removed widget.
	 * @throw morda::Exc if given widget is not a child widget of this container.
	 */
	std::shared_ptr<Widget> remove(Widget& w);

	/**
	 * @brief Remove child from container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param child - iterator of the child to remove.
	 * @return iterator pointing to the next child after removed one.
	 */
	list::const_iterator erase(list::const_iterator child);

	/**
	 * @brief Remove all child widgets.
	 * This function invalidates iterators which were obtained before calling to it.
	 */
	void clear();

	/**
	 * @brief Try get widget by id.
	 * It searches through the whole widget sub-hierarchy, not just direct children of this container.
	 * @param id - id of the child widget to look for.
	 * @return pointer to widget with given id if found.
	 * @return nullptr if there is no widget with given id found.
	 */
	std::shared_ptr<Widget> try_get_widget(const std::string& id)noexcept override;

	/**
	 * @brief Get list of child widgets.
	 * @return List of child widgets.
	 */
	const list& children()noexcept{
		return this->children_v.variable;
	}

	/**
	 * @brief Get constant list of child widgets.
	 * @return Constant list of child widgets.
	 */
	const const_list& children()const noexcept{
		// TRACE(<< "sizeof(list::value_type) = " << sizeof(list::value_type) << std::endl)
		// TRACE(<< "sizeof(const_list::value_type) = " << sizeof(const_list::value_type) << std::endl)
		return this->children_v.constant;
	}

	/**
	 * @brief Find widget in the list of container's children.
	 * @param w - child widget to find.
	 * @return iterator for child widget.
	 * @return end iterator if given widget was not found from the list of container's children.
	 */
	list::const_iterator find(const Widget* w);

	/**
	 * @brief Find widget in the list of container's children.
	 * @param w - child widget to find.
	 * @return iterator for child widget.
	 * @return end iterator if given widget was not found from the list of container's children.
	 */
	const_list::const_iterator find(const Widget* w)const;

	/**
	 * @brief Recursively find all widgets of given type.
	 * @return list of widgets found.
	 */
	template <class T> std::vector<std::shared_ptr<T>> get_all_widgets(){
		std::vector<std::shared_ptr<T>> ret;

		if(auto t = std::dynamic_pointer_cast<T>(this->sharedFromThis(this))){
			ret.emplace_back(t);
		}

		for(auto& child : this->children()){
			if(auto c = std::dynamic_pointer_cast<container>(child)){
				auto res = c->get_all_widgets<T>();
				ret.insert(
						ret.end(),
						std::make_move_iterator(res.begin()),
						std::make_move_iterator(res.end())
					);
			}else if(auto c = std::dynamic_pointer_cast<T>(child)){
				ret.emplace_back(std::move(c));
			}
		}
		return ret;
	}

	/**
	 * @brief Called when children list changes.
	 * This implementation requests re-layout.
     */
	virtual void on_children_changed(){
		this->invalidate_layout();
	}
};




template <class T> T* widget::find_ancestor(const char* id){
	if(!this->parent()){
		return nullptr;
	}

	auto p = dynamic_cast<T*>(this->parent());

	if(p){
		if(!id || p->id == id){
			return p;
		}
	}

	return this->parent()->find_ancestor<T>();
}

//TODO: deprecated, remove.
typedef container Container;

}
