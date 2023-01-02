#pragma once

#include <morda/widgets/group/pile.hpp>

#include "wire_socket.hpp"

class wire_area : public morda::pile{
	friend class wire_socket;
	
	const morda::real deafultWireHalfWidth_c = morda::real(0.25f);
	const uint32_t defaultWireColor_c = 0xff0000ff;
	const uint32_t defaultGrabbedColor_c = 0xff808080;

	morda::real wireHalfWidth = deafultWireHalfWidth_c;
	uint32_t wireColor = defaultWireColor_c;
	uint32_t grabbedColor = defaultGrabbedColor_c;
public:
	wire_area(const wire_area&) = delete;
	wire_area& operator=(const wire_area&) = delete;
	
	wire_area(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	void render(const morda::matrix4& matrix)const override;

	void lay_out() override;
	
	bool on_mouse_move(const morda::mouse_move_event& e)override;

private:
	morda::vector2 mousePos;
	
	std::shared_ptr<wire_socket> grabbedSocket;
	std::shared_ptr<wire_socket> hoveredSocket;
	
	std::vector<utki::shared_ref<wire_socket>> sockets;
};
