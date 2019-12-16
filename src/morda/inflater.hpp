#pragma once

#include <map>
#include <list>
#include <memory>

#include "exception.hpp"

#include "widgets/widget.hpp"



namespace morda{



/**
 * @brief Inflater of GUI from STOB description.
 * This class is used to inflate GUI widget hierarchies from STOB descriptions.
 * The GUI can be described in STOB with the following basic rules:
 * A widget name should start with capital letter. A widget property should start
 * with small letter. Specific widgets define more detailed rules of their description.
 */
class inflater{
	friend class Morda;

	inflater();
public:
	/**
	 * @brief Basic Inflater Exception class.
	 * @param message
	 */
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};

private:
	std::map<std::string, std::function<std::shared_ptr<morda::Widget>(const stob::Node*)> > widgetFactories;

	const decltype(widgetFactories)::value_type::second_type* findFactory(const std::string& widgetName);

	void addWidgetFactory(const std::string& widgetName, decltype(widgetFactories)::value_type::second_type factory);

public:
	//TODO: remove deprecated method
	template <class T_Widget> void addWidget(const std::string& widgetName){
		TRACE(<< "Inflater::addWidget() is DEPRECATED. Use Inflater::registerType() instead." << std::endl)
		this->registerType<T_Widget>(widgetName);
	}

	/**
	 * @brief Registers a new widget type.
	 * Use this function to associate some widget class with a name which can be used
	 * in STOB GUI description.
	 * @param widgetName - name of the widget as it appears in GUI script.
	 */
	template <class T_Widget> void registerType(const std::string& widgetName){
		this->addWidgetFactory(
				widgetName,
				[](const stob::Node* chain) -> std::shared_ptr<morda::Widget> {
					return std::make_shared<T_Widget>(chain);
				}
			);
	}

	/**
	 * @brief Remove previously registered widget type.
	 * @param widgetName - widget name as it appears in GUI script.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given widget name was not found in the list of registered factories.
	 */
	bool removeWidget(const std::string& widgetName)noexcept;

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param chain - GUI script to use.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> inflate(const stob::Node& chain);

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the STOB chain is returned.
	 * @param chain - STOB chain to inflate widget from.
	 * @return reference to the inflated widget.
	 */
	template <typename T> std::shared_ptr<T> inflateAs(const stob::Node& chain){
		return std::dynamic_pointer_cast<T>(this->inflate(chain));
	}

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param str - string containing GUI description.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> inflate(const char* str);

	/**
	 * @brief Inflate widget and cast to specified type.
	 * Only the first widget from the GUI script is returned.
	 * @param str - string holding the GUI script.
	 * @return reference to the inflated widget.
	 */
	template <typename T> std::shared_ptr<T> inflateAs(const char* str){
		return std::dynamic_pointer_cast<T>(this->inflate(str));
	}

	/**
	 * @brief Inflate widget described in GUI script.
	 * @param fi - file interface to get the GUI script.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> inflate(papki::File& fi);

	//TODO: remove includes resolution and this load() method
	/**
	 * @brief Load GUI script.
	 * Loads a GUI script resolving includes.
	 * @param fi - file interface providing GUI script and its dependencies.
	 * @return Pointer to a root node of the GUI hierarchy.
	 */
	static std::unique_ptr<stob::Node> load(papki::File& fi);

private:
	struct Template{
		std::unique_ptr<stob::Node> t;
		std::set<std::string> vars;
	};

	Template parseTemplate(const stob::Node& chain);

	std::list<std::map<std::string, Template>> templates;

	const Template* findTemplate(const std::string& name)const;

	void pushTemplates(const stob::Node& chain);

	void popTemplates();


	//variable name - value mapping
	std::list<std::map<std::string, std::unique_ptr<stob::Node>>> variables;

	const stob::Node* findVariable(const std::string& name)const;

	void pushVariables(const stob::Node& chain);

	void popVariables();

	void substituteVariables(stob::Node* to)const;

	void pushDefs(const stob::Node& chain);
	void popDefs();
};



}
