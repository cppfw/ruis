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

#pragma once

#include <map>
#include <vector>

#include "../util/util.hpp"
#include "widget.hpp"

namespace morda{

/**
 * @brief Container widget.
 * A widget which can contain children widgets.
 * From GUI script it can be instantiated as 'container' and can have child widgets specified. For example:
 * @code
 * @container{
 *     @text{
 *         text{"Hello world!"}
 *     }
 *     @text{
 *         text{"Hi!"}
 *     }
 * }
 * @endcode
 */
class container : virtual public widget{
public:
	typedef std::vector<std::shared_ptr<widget>> widget_list;
	typedef std::vector<std::shared_ptr<const widget>> const_widget_list;
private:
	static_assert(sizeof(widget_list) == sizeof(const_widget_list), "sizeof(widget_list) differs from sizeof(const_widget_list)");
	static_assert(sizeof(widget_list::value_type) == sizeof(const_widget_list::value_type), "sizeof(widget_list::value_type) differs from sizeof(const_widget_list::value_type)");

	// NOTE: according to C++11 standard it is undefined behaviour to read the inactive union member,
	//       but we rely on compiler implementing it the right way.
	union children_union{
		widget_list variable;
		const_widget_list constant; // this member never becomes active one, but we will read it when we need constant list of children

		children_union() :
				variable() // this sets the 'variable' member of the union as an active one
		{}
		~children_union(){
			this->variable.~widget_list();
		}
	} children_list;

	struct mouse_capture_info{
		std::weak_ptr<widget> capturing_widget;
		unsigned num_buttons_captured;
	};

	// map which maps pointer ID to a pair holding reference to capturing widget and number of mouse capture clicks
	std::map<unsigned, mouse_capture_info> mouse_capture_map;

private:
	// flag indicating that modifications to children list are blocked
	bool is_blocked = false;

	class blocked_flag_guard{
		bool& blocked;
	public:
		blocked_flag_guard(bool& blocked) :
				blocked(blocked)
		{
			this->blocked = true;
		}

		~blocked_flag_guard()noexcept{
			this->blocked = false;
		}
	};

protected:
	/**
	 * @brief Create layout parameters object specific for this container.
	 * Other widgets which subclass container and need additional parameters for laying out children widgets should extend the widget::layout_params class
	 * and override this method to construct the correct layout_params instance.
	 * @param desc - description of the layout parameters.
	 * @return A new instance of layout parameters object.
	 */
	virtual std::unique_ptr<layout_params> create_layout_params(const treeml::forest& desc)const;

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

public:

	/**
	 * @brief Get layout parameters of child widget.
	 * Same as get_layout_params() but also tries to cast the layout_params object to specified class.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
	template <class T> const T& get_layout_params_as_const(const widget& w)const{
		auto p = dynamic_cast<const T*>(&this->get_layout_params_const(w));
		if(!p){
			w.layout_params_.reset();
			p = dynamic_cast<const T*>(&this->get_layout_params_const(w));
		}

		if(!p){
			throw std::bad_cast();
		}
		return *p;
	}

	/**
	 * @brief Get layout parameters of child widget.
	 * Same as get_layout_params() but also tries to cast the layout_params object to specified class.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 */
	template <class T> T& get_layout_params_as(widget& w){
		auto p = dynamic_cast<T*>(&this->get_layout_params(w));
		if(!p){
			w.layout_params_.reset();
			p = dynamic_cast<T*>(&this->get_layout_params(w));
		}

		if(!p){
			throw std::bad_cast();
		}
		return *p;
	}

	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 * @throw std::invalid_argument - in case the given widget is not a child of this container.
	 */
	layout_params& get_layout_params(widget& w);

	/**
	 * @brief Get layout parameters of child widget.
	 * @param w - widget to get layout parameters for.
	 * @return Layout parameters of given child widget.
	 * @throw std::invalid_argument - in case the given widget is not a child of this container.
	 */
	const layout_params& get_layout_params_const(const widget& w)const;

	using widget::get_layout_params;

protected:

	void render_child(const matrix4& matrix, const widget& c)const;
public:
	/**
	 * @brief Constructor.
	 * @param c - context to which this widget belongs.
	 * @param desc - description of the widget.
	 */
	container(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	void render(const matrix4& matrix)const override;

	bool on_mouse_button(const mouse_button_event& event)override;

	bool on_mouse_move(const mouse_move_event& event)override;

	void on_hover_change(unsigned pointer_id)override;

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
	widget_list::const_iterator change_child_z_position(widget_list::const_iterator child, widget_list::const_iterator before);

	/**
	 * @brief Insert a widget to the container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param w - widget to insert.
	 * @param before - iterator within this container before which the widget will be inserted.
	 * @return iterator pointing to the newly inserted widget.
	 */
	widget_list::const_iterator insert(std::shared_ptr<widget> w, widget_list::const_iterator before);

	/**
	 * @brief Insert a widget to the end of children list of the container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param w - widget to insert.
	 * @return iterator pointing to the newly inserted widget.
	 */
	widget_list::const_iterator push_back(std::shared_ptr<widget> w){
		return this->insert(std::move(w), this->children().end());
	}

	/**
	 * @brief Add child widgets inflating them from GUI description.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param desc - GUI description of the widgets to add.
	 */
	void push_back_inflate(const treeml::forest& desc);

	/**
	 * @brief Remove last child.
	 */
	void pop_back(){
		if(this->children().empty()){
			return;
		}
		this->erase(--this->children().end());
	}

	/**
	 * @brief Remove child from container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param child - iterator of the child to remove.
	 * @return iterator pointing to the next child after removed one.
	 */
	widget_list::const_iterator erase(widget_list::const_iterator child);

	/**
	 * @brief Remove child from container.
	 * This function invalidates iterators which were obtained before calling to it.
	 * @param child - reverse iterator of the child to remove.
	 * @return reverse iterator pointing to the previous child after removed one.
	 */
	widget_list::const_reverse_iterator erase(widget_list::const_reverse_iterator child){
		return widget_list::const_reverse_iterator(this->erase(--child.base())); // the base iterator points to the next element to the one the reverse iterator points, so use decrement
	}

	/**
	 * @brief Remove all child widgets.
	 * This function invalidates iterators which were obtained before calling to it.
	 */
	void clear();

	/**
	 * @brief Try get widget by id.
	 * It searches through the whole widget sub-hierarchy, not just direct children of this container.
	 * @param id - id of the child widget to look for.
	 * @param allow_itself - whether it is allowed to return itself in case id matches.
	 * @return pointer to widget with given id if found.
	 * @return nullptr if there is no widget with given id found.
	 */
	std::shared_ptr<widget> try_get_widget(const std::string& id, bool allow_itself = true)noexcept override final;

	/**
	 * @brief Get list of child widgets.
	 * @return List of child widgets.
	 */
	const widget_list& children()noexcept{
		return this->children_list.variable;
	}

	/**
	 * @brief Get constant list of child widgets.
	 * @return Constant list of child widgets.
	 */
	const const_widget_list& children()const noexcept{
		// TRACE(<< "sizeof(widget_list::value_type) = " << sizeof(widget_list::value_type) << std::endl)
		// TRACE(<< "sizeof(const_widget_list::value_type) = " << sizeof(const_widget_list::value_type) << std::endl)
		return this->children_list.constant;
	}

	/**
	 * @brief Find widget in the list of container's children.
	 * @param w - child widget to find.
	 * @return iterator for child widget.
	 * @return end iterator if given widget was not found from the list of container's children.
	 */
	widget_list::const_iterator find(const widget& w);

	/**
	 * @brief Find widget in the list of container's children.
	 * @param w - child widget to find.
	 * @return iterator for child widget.
	 * @return end iterator if given widget was not found from the list of container's children.
	 */
	const_widget_list::const_iterator find(const widget& w)const;

	/**
	 * @brief Get begin iterator into the children list.
	 * @return begin iterator into the children list.
	 */
	widget_list::const_iterator begin(){
		return this->children().begin();
	}

	/**
	 * @brief Get end iterator into the children list.
	 * @return end iterator into the children list.
	 */
	widget_list::const_iterator end(){
		return this->children().end();
	}

	/**
	 * @brief Get const begin iterator into the children list.
	 * @return const begin iterator into the children list.
	 */
	const_widget_list::const_iterator begin()const{
		return this->children().begin();
	}

	/**
	 * @brief Get const end iterator into the children list.
	 * @return const end iterator into the children list.
	 */
	const_widget_list::const_iterator end()const{
		return this->children().end();
	}

	/**
	 * @brief Get first child widget.
	 * @return first child widget.
	 */
	const widget_list::value_type& front(){
		return this->children().front();
	}

	/**
	 * @brief Get const first child widget.
	 * @return const first child widget.
	 */
	const const_widget_list::value_type& front()const{
		return this->children().front();
	}

	/**
	 * @brief Get last child widget.
	 * @return last child widget.
	 */
	const widget_list::value_type& back(){
		return this->children().back();
	}

	/**
	 * @brief Get const last child widget.
	 * @return const last child widget.
	 */
	const const_widget_list::value_type& back()const{
		return this->children().back();
	}

	/**
	 * @brief Get number of child widgets.
	 * @return number of child widgets.
	 */
	size_t size()const noexcept{
		return this->children().size();
	}

	/**
	 * @brief Check if container has no children.
	 * @return true in case the container has no any child widgets.
	 * @return false otherwise.
	 */
	bool empty()const noexcept{
		return this->size() == 0;
	}

	/**
	 * @brief Called when children list changes.
	 * This implementation invalidates layout.
     */
	virtual void on_children_change(){
		this->invalidate_layout();
	}

	/**
	 * @brief Handler of enable state change.
	 * This implementation sets the same enabled state to all children of the container.
	 */
	void on_enable_change()override;
};

template <class T>
T* widget::try_get_ancestor(const std::string& id){
	if(!this->parent()){
		return nullptr;
	}

	auto p = dynamic_cast<T*>(
		// down-cast to widget* because container can be privately inherited by T
		// and in this case dynamic_cast to T* will fail
		static_cast<widget*>(this->parent())
	);

	if(p){
		if(id.empty() || p->id == id){
			return p;
		}
	}

	return this->parent()->try_get_ancestor<T>(id);
}

template <typename T>
std::shared_ptr<T> widget::try_get_widget(bool allow_itself)noexcept{
	if(allow_itself){
		auto p = std::dynamic_pointer_cast<T>(utki::make_shared_from(*this));
		if(p){
			return p;
		}
	}

	auto cont = dynamic_cast<container*>(this);
	if(cont){
		for(const auto& c : cont->children()){
			auto w = c->try_get_widget<T>(true);
			if(w){
				return w;
			}
		}
	}

	return nullptr;
}

template <typename T> T& widget::get_widget(bool allow_itself){
	auto p = this->try_get_widget<T>(allow_itself);
	if(p){
		return *p;
	}

	throw std::logic_error("widget::get_widget_as(): requested widget type is not found");
}

template <class T> std::vector<std::shared_ptr<T>> widget::get_all_widgets(bool allow_itself){
	std::vector<std::shared_ptr<T>> ret;

	if(allow_itself){
		if(auto p = dynamic_cast<T*>(this)){
			ret.push_back(utki::make_shared_from(*p));
		}
	}

	if(auto cont = dynamic_cast<container*>(this)){
		for(auto& c : cont->children()){
			auto res = c->get_all_widgets<T>(true);
			ret.insert(
					ret.end(),
					std::make_move_iterator(res.begin()),
					std::make_move_iterator(res.end())
				);
		}
	}
	
	return ret;
}

}
