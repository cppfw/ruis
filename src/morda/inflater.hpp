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
#include <memory>

#include "widgets/widget.hpp"

#include "util/util.hpp"

namespace morda{

/**
 * @brief Inflater of GUI from treeml description.
 * This class is used to inflate GUI widget hierarchies from treeml descriptions.
 * The GUI can be described in treeml with the following basic rules:
 * A widget name should start with capital letter. A widget property should start
 * with small letter. Specific widgets define more detailed rules of their description.
 */
class inflater{
	friend class context;

	morda::context& context;

	inflater(morda::context& context);

private:
	std::map<
			std::string,
			std::function<
					utki::shared_ref<morda::widget>(
							const utki::shared_ref<morda::context>&,
							const treeml::forest&
						)
				>
		>
	factories;

	const decltype(factories)::value_type::second_type& find_factory(const std::string& widget_name);

	void add_factory(std::string&& widget_name, decltype(factories)::value_type::second_type&& factory);

public:
	/**
	 * @brief Registers a new widget type.
	 * Use this function to associate some widget class with a name which can be used
	 * in STOB GUI description.
	 * @param widget_name - name of the widget as it appears in GUI script.
	 */
	template <class T> void register_widget(const std::string& widget_name){
		this->add_factory(
				std::string(widget_name),
				[](const utki::shared_ref<morda::context>& c, const treeml::forest& desc) -> utki::shared_ref<morda::widget> {
					return utki::make_shared_ref<T>(c, desc);
				}
			);
	}

	/**
	 * @brief Remove previously registered widget type.
	 * @param widget_name - widget name as it appears in GUI script.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given widget name was not found in the list of registered factories.
	 */
	bool unregister_widget(const std::string& widget_name)noexcept;

	// TODO: doxygen
	void push_defs(const treeml::forest& chain);

	// TODO: doxygen
	void push_defs(treeml::forest::const_iterator begin, treeml::forest::const_iterator end);

	// TODO: doxygen
	void push_defs(const char* str);

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param begin - begin iterator into the GUI script.
	 * @param end - begin iterator into the GUI script.
	 * @return the inflated widget.
	 */
	utki::shared_ref<widget> inflate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end);

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param gui_script - GUI script to use.
	 * @return the inflated widget.
	 */
	utki::shared_ref<widget> inflate(const treeml::forest& gui_script){
		return this->inflate(gui_script.begin(), gui_script.end());
	}

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the treeml chain is returned.
	 * @param gui_script - gui script to inflate widget from.
	 * @return the inflated widget.
	 */
	template <typename T> utki::shared_ref<T> inflate_as(const treeml::forest& gui_script){
		return utki::dynamic_reference_cast<T>(this->inflate(gui_script));
	}

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param str - string containing GUI description.
	 * @return the inflated widget.
	 */
	utki::shared_ref<widget> inflate(const char* str);

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param str - string containing GUI description.
	 * @return the inflated widget.
	 */
	utki::shared_ref<widget> inflate(const std::string& str);

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the GUI script is returned.
	 * @param str - string of the GUI script.
	 * @return the inflated widget.
	 */
	template <typename T> utki::shared_ref<T> inflate_as(const char* str){
		return utki::dynamic_reference_cast<T>(this->inflate(str));
	}

	/**
	 * @brief Inflate widget described in GUI script.
	 * @param fi - file interface to get the GUI script.
	 * @return the inflated widget.
	 */
	utki::shared_ref<morda::widget> inflate(const papki::file& fi);

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the GUI script is returned.
	 * @param fi - file interface to get the GUI script.
	 * @return the inflated widget.
	 */
	template <typename T> utki::shared_ref<T> inflate_as(const papki::file& fi){
		return utki::dynamic_reference_cast<T>(this->inflate(fi));
	}

private:
	struct widget_template{
		treeml::tree templ;
		std::set<std::string> vars;
	};

	widget_template parse_template(const std::string& name, const treeml::forest& chain);

	std::vector<std::map<std::string, widget_template>> templates;

	const widget_template* find_template(const std::string& name)const;

	void push_templates(const treeml::forest& chain);

	void pop_templates();

	// variable name-value mapping
	std::vector<std::map<std::string, treeml::forest>> variables;

	const treeml::forest* find_variable(const std::string& name)const;

	void push_variables(const treeml::forest& trees);
	void pop_variables();

	void push_defs_block(const treeml::forest& chain);
	void pop_defs_block();
};

}
