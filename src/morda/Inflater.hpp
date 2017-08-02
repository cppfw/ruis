#pragma once

#include <map>
#include <memory>

#include "Exc.hpp"

#include "widgets/core/Widget.hpp"



namespace morda{



/**
 * @brief Inflater of GUI from STOB description.
 * This class is used to inflate GUI widget hierarchies from STOB descriptions.
 * The GUI can be described in STOB with the following basic rules:
 * A widget name should start with capital letter. A widget property should start
 * with small letter. Specific widgets define more detailed rules of their description.
 */
class Inflater{
	friend class Morda;

	Inflater();
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
	class WidgetFactory{
	public:
		virtual std::shared_ptr<morda::Widget> create(const stob::Node* chain)const = 0;

		virtual ~WidgetFactory()noexcept{}
	};
	
	typedef std::map<std::string, std::unique_ptr<WidgetFactory> > T_FactoryMap;
	T_FactoryMap widgetFactories;

	const WidgetFactory* findFactory(const std::string& widgetName);
	
	void addWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory);

public:
	/**
	 * @brief Registers a new widget type.
	 * Use this function to associate some widget class with a name which can be used
	 * in STOB GUI description.
	 * @param widgetName - name of the widget as it appears in GUI script.
	 */
	template <class T_Widget> void addWidget(const std::string& widgetName){
		class Factory : public WidgetFactory{
		public:
			std::shared_ptr<morda::Widget> create(const stob::Node* chain)const override{
				return utki::makeShared<T_Widget>(chain);
			}
		};

		this->addWidgetFactory(widgetName, std::unique_ptr<WidgetFactory>(new Factory()));
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
	 * @brief Inflate widget described in GUI script.
	 * @param fi - file interface to get the GUI script.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> inflate(papki::File& fi);

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
};



}//~namespace
