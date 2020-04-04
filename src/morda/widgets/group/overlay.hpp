#pragma once

#include "pile.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 * Overlay container is used for displaying topmost widgets like context menus, hint popups etc.
 * Essentially, the overlay is a simple pile container which keeps track of open overlay widgets.
 * From GUI scripts it can be instantiated as "overlay".
 */
class overlay :
		virtual public widget,
		public pile
{
public:
	overlay(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	overlay(const overlay&) = delete;
	overlay& operator=(const overlay&) = delete;
	
	/**
	 * @brief Showing context menu.
	 * This function adds the context menu widget to the overlay as the topmost widget.
	 * The given context menu widget is wrapped into special overhead container to make it properly positioned
	 * on the screen and to handle mouse clicks outside of the context menu widget
	 * (i.e. close the context menu in case of mouse click outside).
	 * @param menu - context menu widget.
	 * @param pos - position of top left corner of the context menu within the overlay container.
	 * @return the final widget added to the overlay pile. This widget can be used to later close the particular context menu
	 *         by just removing the widget from its parent.
	 */
	std::shared_ptr<widget> show_context_menu(std::shared_ptr<widget> menu, vector2 pos);
	
	/**
	 * @brief Close all context menus.
	 */
	void close_all_context_menus();
private:

};

}
