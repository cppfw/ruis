/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>
#include <memory>

#include <ting/util.hpp>

#include "Exc.hpp"
#include "widgets/Widget.hpp"



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
		virtual std::shared_ptr<morda::Widget> Create(const stob::Node* chain)const = 0;

		virtual ~WidgetFactory()NOEXCEPT{}
	};

private:
	typedef std::map<std::string, std::unique_ptr<WidgetFactory> > T_FactoryMap;
	T_FactoryMap widgetFactories;

	void AddWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory);

public:
	/**
	 * @brief Registers a new widget type.
	 * @param widgetName - name of the widget as it appears in GUI script.
	 */
	template <class T_Widget> void AddWidget(const std::string& widgetName){
		class Factory : public WidgetFactory{
		public:
			std::shared_ptr<morda::Widget> Create(const stob::Node* chain)const override{
				return ting::New<T_Widget>(chain);
			}
		};

		this->AddWidgetFactory(widgetName, std::unique_ptr<WidgetFactory>(new Factory()));
	}

	/**
	 * @brief Remove previously registered widget type.
	 * @param widgetName - widget name as it appears in GUI script.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given widget name was not found in the list of registered factories.
	 */
	bool RemoveWidget(const std::string& widgetName)NOEXCEPT;

	/**
	 * @brief Create widgets hierarchy from GUI script.
	 * @param chain - GUI script to use.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> Inflate(const stob::Node& chain);

	/**
	 * @brief Inflate widget described in GUI script.
	 * @param fi - file interface to get the GUI script from.
	 * @return reference to the inflated widget.
	 */
	std::shared_ptr<morda::Widget> Inflate(ting::fs::File& fi);

	/**
	 * @brief Load GUI script.
	 * Loads a GUI script resolving includes.
	 * @param fi - file interface providing GUI script and its dependencies.
	 * @return Pointer to a root node of the GUI hierarchy.
	 */
	static std::unique_ptr<stob::Node> Load(ting::fs::File& fi);
	
private:
	std::list<std::map<std::string, std::unique_ptr<stob::Node>>> templates;
	
	const stob::Node* FindTemplate(const std::string& name)const;
public:
	
	//returns true if templates were pushed to templates stack
	bool PushTemplates(std::unique_ptr<stob::Node> chain);
	
	void PopTemplates();
};



}//~namespace
