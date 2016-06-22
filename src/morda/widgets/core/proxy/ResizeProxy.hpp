/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../Widget.hpp"


namespace morda{

class ResizeProxy : virtual public Widget{
public:
	ResizeProxy(const stob::Node* chain = nullptr) :
			Widget(chain)
	{}
	
	ResizeProxy(const ResizeProxy&) = delete;
	ResizeProxy& operator=(const ResizeProxy&) = delete;
	
	void onResize() override;

	std::function<void(const Vec2r&)> resized;
};

}
