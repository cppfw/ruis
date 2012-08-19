/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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

#include "Widget.hpp"


// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <map>

#include "Exc.hpp"
#include "Widget.hpp"
#include "Layout.hpp"



namespace morda{



class App;



class GuiInflater{
	friend class morda::App;
	
	GuiInflater();
public:
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};
	
	class WidgetFactory{
	public:
		virtual ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const = 0;

		virtual ~WidgetFactory()throw(){}
	};
	
	class LayoutFactory{
	public:
		virtual ting::Ptr<morda::Layout> Create(const stob::Node& node)const = 0;

		virtual ~LayoutFactory()throw(){}
	};
	
private:
	typedef std::map<std::string, ting::Ptr<WidgetFactory> > T_FactoryMap;
	T_FactoryMap widgetFactories;
	
	typedef std::map<std::string, ting::Ptr<LayoutFactory> > T_LayoutMap;
	T_LayoutMap layoutFactories;
	
public:
	/**
	 * @brief Registers a new factory for constructing widgets.
     * @param widgetName - name of the widget the factory constructs.
     * @param factory - factory object.
     */
	void AddWidgetFactory(const std::string& widgetName, ting::Ptr<WidgetFactory> factory);
	
	/**
	 * @brief Remove widget factory.
     * @param widgetName - widget name associated with the factory to remove.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given widget name was not found in the list of registered factories.
     */
	bool RemoveWidgetFactory(const std::string& widgetName)throw();
	
	/**
	 * @brief Create widgets hierarchy from GUI script.
     * @param gui - GUI script to use.
     * @return reference to the root widget of the created hierarchy.
     */
	ting::Ref<morda::Widget> Inflate(ting::Ptr<stob::Node> gui)const;
	
	/**
	 * @brief Create widgets hierarchy from GUI script.
     * @param fi - file interface to get the GUI script from.
     * @return reference to the root widget of the created hierarchy.
     */
	ting::Ref<morda::Container> Inflate(ting::fs::File& fi)const;
	
	/**
	 * @brief Registers a layout factory.
     * @param layoutName - name of the layout the factory creates.
     * @param factory - the factory object to register.
     */
	void AddLayoutFactory(const std::string& layoutName, ting::Ptr<LayoutFactory> factory);
	
	/**
	 * @brief Remove layout factory.
     * @param layoutName - name of the layout associated with the factory.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given layout name was not found in the list of registered factories.
     */
	bool RemoveLayoutFactory(const std::string& layoutName)throw();
	
	
	ting::Ptr<Layout> CreateLayout(const stob::Node& layout)const;
};



}//~namespace
