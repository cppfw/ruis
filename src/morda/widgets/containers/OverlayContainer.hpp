/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "FrameContainer.hpp"

namespace morda{

/**
 * @brief Overlay container for displaying widgets on top of anything.
 */
class OverlayContainer :
		virtual public Widget,
		private FrameContainer
{
	std::shared_ptr<Container> overlayContainer;
	std::shared_ptr<FrameContainer> contentContainer;
public:
	OverlayContainer(const stob::Node* chain = nullptr);
	
	OverlayContainer(const OverlayContainer&) = delete;
	OverlayContainer& operator=(const OverlayContainer&) = delete;
	
	Container& overlay(){
		return *this->overlayContainer;
	}
	
	FrameContainer& content(){
		return *this->contentContainer;
	}
	
	void showContextMenu(std::shared_ptr<Widget> menu, Vec2r anchor);
	
private:

};

}
