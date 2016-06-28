/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Frame.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 */
class Overlay : public Frame{
	std::shared_ptr<Widget> overlayLayer;
	std::shared_ptr<Container> overlayContainer;
public:
	Overlay(const stob::Node* chain = nullptr);
	
	Overlay(const Overlay&) = delete;
	Overlay& operator=(const Overlay&) = delete;
	
	Container& overlay(){
		return *this->overlayContainer;
	}
	
	void showContextMenu(std::shared_ptr<Widget> menu, Vec2r anchor);
	
	void hideContextMenu();
	
	void onChildrenListChanged() override;

private:

};

}
