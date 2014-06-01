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

#include <ting/util.hpp>

#include "Exc.hpp"
#include "widgets/Widget.hpp"
#include "layouts/Layout.hpp"



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
		virtual ting::Ref<morda::Widget> Create(const stob::Node& node)const = 0;

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
	
	void AddWidgetFactory(const std::string& widgetName, ting::Ptr<WidgetFactory> factory);
	
	typedef std::map<std::string, ting::Ptr<LayoutFactory> > T_LayoutMap;
	T_LayoutMap layoutFactories;
	
	void AddLayoutFactory(const std::string& layoutName, ting::Ptr<LayoutFactory> factory);
	
public:
	/**
	 * @brief Registers a new widget type.
     * @param widgetName - name of the widget as it appears in GUI script.
     */
	template <class T_Widget> void AddWidget(const std::string& widgetName){
		class Factory : public WidgetFactory{
		public:
			ting::Ref<morda::Widget> Create(const stob::Node& node)const OVERRIDE{
				return T_Widget::New(node);
			}
		};
		
		this->AddWidgetFactory(widgetName, ting::Ptr<WidgetFactory>(new Factory()));
	}
	
	/**
	 * @brief Remove previously registered widget type.
     * @param widgetName - widget name as it appears in GUI script.
	 * @return true if factory was successfully removed.
	 * @return false if the factory with given widget name was not found in the list of registered factories.
     */
	bool RemoveWidget(const std::string& widgetName)throw();
	
	/**
	 * @brief Create widgets hierarchy from GUI script.
     * @param gui - GUI script to use.
     * @return reference to the inflated widget.
     */
	ting::Ref<morda::Widget> Inflate(const stob::Node& gui)const;
	
	/**
	 * @brief Inflate widget described in GUI script.
     * @param fi - file interface to get the GUI script from.
     * @return reference to the inflated widget.
     */
	ting::Ref<morda::Widget> Inflate(ting::fs::File& fi)const;
	
	/**
	 * @brief Registers a layout type.
     * @param layoutName - name of the layout as it appears in GUI script.
     */
	template <class T_Layout> void AddLayout(const std::string& layoutName){
		class Factory : public LayoutFactory{
		public:
			ting::Ptr<morda::Layout> Create(const stob::Node& node)const OVERRIDE{
				return T_Layout::New(node);
			}
		};

		this->AddLayoutFactory(layoutName, ting::Ptr<LayoutFactory>(new Factory()));
	}
	
	/**
	 * @brief Remove previously registered layout type.
     * @param layoutName - name of the layout as it appears in GUI script.
	 * @return true if layout type was successfully removed.
	 * @return false if layout type with given name was not found in the list of registered types.
     */
	bool RemoveLayout(const std::string& layoutName)throw();
	
	
	ting::Ptr<Layout> CreateLayout(const stob::Node& layout)const;
};



}//~namespace
