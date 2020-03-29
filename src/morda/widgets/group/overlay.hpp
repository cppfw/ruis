#pragma once

#include "pile.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 * Overlay container is used for displaying topmost widgets like context menus, hint popups etc.
 * From GUI scripts it can be instantiated as "overlay".
 */
class overlay : public pile{
	std::shared_ptr<widget> overlayLayer;
	std::shared_ptr<container> overlayContainer;
public:
	overlay(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	overlay(const overlay&) = delete;
	overlay& operator=(const overlay&) = delete;
	
	/**
	 * @brief get topmost container.
	 * This function returns the simple container object which resides on top of all normal children of this overlay container.
	 * By adding child widget to this topmost container those widgets will be on top of everything.
	 * @return overlay container.
	 */
	container& top_layer(){
		return *this->overlayContainer;
	}
	
	/**
	 * @brief Convenience method for showing context menu.
	 * @param menu - context menu widget.
	 * @param anchor - anchor point, position of top left corner of the context menu within the overlay container.
	 */
	void show_context_menu(std::shared_ptr<widget> menu, vector2 anchor);
	
	/**
	 * @brief Hide context menu.
	 */
	void hide_context_menu();
	
	void on_children_changed() override;

private:

};

}
