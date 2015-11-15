/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "FrameContainer.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 */
class OverlayContainer : public FrameContainer{
	std::shared_ptr<Widget> overlayLayer;
	std::shared_ptr<Container> overlayContainer;
public:
	OverlayContainer(const stob::Node* chain = nullptr);
	
	OverlayContainer(const OverlayContainer&) = delete;
	OverlayContainer& operator=(const OverlayContainer&) = delete;
	
	Container& overlay(){
		return *this->overlayContainer;
	}
	
	void showContextMenu(std::shared_ptr<Widget> menu, Vec2r anchor);
	
	void hideContextMenu();
	
	void onChildrenListChanged() override;

private:

};

}
