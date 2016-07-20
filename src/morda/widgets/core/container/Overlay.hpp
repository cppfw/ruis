#pragma once

#include "Frame.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 * Overlay container is used for displaying topmost widgets like context menus, hint popups etc.
 * From GUI scripts it can be instantiated as "Overlay".
 */
class Overlay : public Frame{
	std::shared_ptr<Widget> overlayLayer;
	std::shared_ptr<Container> overlayContainer;
public:
	Overlay(const stob::Node* chain = nullptr);
	
	Overlay(const Overlay&) = delete;
	Overlay& operator=(const Overlay&) = delete;
	
	/**
	 * @brief get topmost container.
	 * This gets the simple Container object which resides on top of all normal children of this Overlay container.
	 * By adding child widget to this topmost container those widgets will be on top of everything.
	 * @return Overlay container.
	 */
	Container& overlay(){
		return *this->overlayContainer;
	}
	
	/**
	 * @brief Convenience method for showing context menu.
	 * @param menu - context menu widget.
	 * @param anchor - anchor point, position of top left corner of the context menu within the overlay container.
	 */
	void showContextMenu(std::shared_ptr<Widget> menu, Vec2r anchor);
	
	/**
	 * @brief Hide context menu.
	 */
	void hideContextMenu();
	
	void onChildrenListChanged() override;

private:

};

}
