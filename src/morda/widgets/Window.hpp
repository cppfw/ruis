/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "label/TextLabel.hpp"
#include "label/ColorLabel.hpp"

#include "core/container/Frame.hpp"
#include "core/proxy/MouseProxy.hpp"
#include "core/container/Table.hpp"

namespace morda{



class Window :
		virtual public Widget,
		private Frame
{
	morda::Vec2r emptyMinDim;//minimal dimension of empty window
	
	std::shared_ptr<ColorLabel> titleBg;
	std::uint32_t titleBgColorTopmost;
	std::uint32_t titleBgColorNonTopmost;
	
	std::shared_ptr<TextLabel> title;
	
	std::shared_ptr<Frame> contentArea;
	
	std::shared_ptr<Widget> tBorder;
	std::shared_ptr<Widget> lBorder;
	std::shared_ptr<Widget> rBorder;
	std::shared_ptr<Widget> bBorder;
	
	bool captionCaptured = false;
	bool leftTopResizeCaptured = false;
	bool leftBottomResizeCaptured = false;
	bool rightTopResizeCaptured = false;
	bool rightBottomResizeCaptured = false;
	bool leftResizeCaptured = false;
	bool rightResizeCaptured = false;
	bool topResizeCaptured = false;
	bool bottomResizeCaptured = false;
	
	morda::Vec2r capturePoint;
	
	void setupWidgets();
	
public:
	Window(const stob::Node* chain = nullptr);
	
	
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	
	void setTitle(const std::string& str);
	
	Frame& content(){
		return *this->contentArea;
	}
	

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerId)override;

	void onTopmostChanged()override;
	
	void setBackground(std::shared_ptr<Widget> w);
	
	void setBorders(Sidesr borders);
};

}
