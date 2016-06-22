/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>
#include <memory>

#include "Exc.hpp"
#include "widgets/core/Widget.hpp"



namespace morda{



class App;



class Inflater{
	friend class morda::App;

	Inflater();
public:
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};

	class WidgetFactory{
	public:
		virtual std::shared_ptr<morda::Widget> create(const stob::Node* chain)const = 0;

		virtual ~WidgetFactory()noexcept{}
	};

private:
	typedef std::map<std::string, std::unique_ptr<WidgetFactory> > T_FactoryMap;
	T_FactoryMap widgetFactories;

	void addWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory);

public:
	/**
	 * @brief Registers a new widget type.
	 * @param widgetName - name of the widget as it appears in GUI script.
	 */
	template <class T_Widget> void AddWidget(const std::string& widgetName){
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
	 * @param fi - file interface to get the GUI script from.
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
	std::list<std::map<std::string, std::unique_ptr<stob::Node>>> templates;
	
	const stob::Node* findTemplate(const std::string& name)const;
	
	void pushTemplates(std::unique_ptr<stob::Node> chain);
	
	void popTemplates();
	
	
	//variable name - value mapping
	std::list<std::map<std::string, std::string>> variables;
	
	const std::string* findVariable(const std::string& name)const;
	
	void pushVariables(const stob::Node& chain);
	
	void popVariables();
	
	void substituteVariables(stob::Node* to)const;
};



}//~namespace
