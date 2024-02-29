#pragma once

#include "wire_socket.hpp"

class wire_area : public ruis::container{
	friend class wire_socket;
	
	const ruis::real deafult_wire_half_width = ruis::real(0.25f);
	const uint32_t default_wire_color = 0xff0000ff;
	const uint32_t default_grabbed_color = 0xff808080;

	ruis::real wire_half_width = deafult_wire_half_width;
	uint32_t wire_color = default_wire_color;
	uint32_t grabbed_color = default_grabbed_color;
public:
	wire_area(const wire_area&) = delete;
	wire_area& operator=(const wire_area&) = delete;

	wire_area(wire_area&&) = delete;
	wire_area& operator=(wire_area&&) = delete;

	~wire_area() override = default;
	
	wire_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);
	
	void render(const ruis::matrix4& matrix)const override;

	void on_lay_out() override;
	
	bool on_mouse_move(const ruis::mouse_move_event& e)override;

private:
	ruis::vector2 mouse_pos;
	
	std::shared_ptr<wire_socket> grabbed_socket;
	std::shared_ptr<wire_socket> hovered_socket;
	
	std::vector<utki::shared_ref<wire_socket>> sockets;
};
