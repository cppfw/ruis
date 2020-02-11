#pragma once

#include <map>
#include <memory>

#include "exception.hpp"

#include "widgets/widget.hpp"

#include "util/util.hpp"


namespace morda{



/**
 * @brief Inflater of GUI from puu description.
 * This class is used to inflate GUI widget hierarchies from puu descriptions.
 * The GUI can be described in puu with the following basic rules:
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
					std::shared_ptr<morda::widget>(
							const std::shared_ptr<morda::context>&,
							const puu::forest&
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
				[](const std::shared_ptr<morda::context>& c, const puu::forest& desc) -> std::shared_ptr<morda::widget> {
					return std::make_shared<T>(c, desc);
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

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param begin - begin iterator into the GUI script.
	 * @param end - begin iterator into the GUI script.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<widget> inflate(puu::forest::const_iterator begin, puu::forest::const_iterator end);

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param gui_script - GUI script to use.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<widget> inflate(const puu::forest& gui_script){
		return this->inflate(gui_script.begin(), gui_script.end());
	}
		

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the STOB chain is returned.
	 * @param gui_script - gui script to inflate widget from.
	 * @return reference to the inflated widget.
	 */
	template <typename T> std::shared_ptr<T> inflate_as(const puu::forest& gui_script){
		return std::dynamic_pointer_cast<T>(this->inflate(gui_script));
	}


	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param str - string containing GUI description.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<widget> inflate(const char* str);

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the GUI script is returned.
	 * @param str - string of the GUI script.
	 * @return reference to the inflated widget.
	 */
	template <typename T> std::shared_ptr<T> inflate_as(const char* str){
		return std::dynamic_pointer_cast<T>(this->inflate(str));
	}

	/**
	 * @brief Inflate widget described in GUI script.
	 * @param fi - file interface to get the GUI script.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> inflate(const papki::File& fi);

private:
	struct widget_template{
		puu::tree templ;
		std::set<std::string> vars;
	};

	widget_template parse_template(const puu::forest& chain);

	std::vector<std::map<std::string, widget_template>> templates;

	const widget_template* find_template(const std::string& name)const;

	void push_templates(const puu::forest& chain);

	void pop_templates();

	// variable name-value mapping
	std::vector<std::map<std::string, puu::forest>> variables;

	const puu::forest* find_variable(const std::string& name)const;

	void push_variables(const puu::forest& trees);

	void pop_variables();

	void push_defs(const puu::forest& chain);
	void pop_defs();
};



}
