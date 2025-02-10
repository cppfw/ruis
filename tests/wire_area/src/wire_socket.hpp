#pragma once

#include <ruis/widget/widget.hpp>

namespace ruis{

class wire_socket : virtual public ruis::widget{
	friend class wire_area;

public:
	enum class orientation{
		left,
		top,
		right,
		bottom,

		enum_size
	};

	struct parameters{
		orientation outlet_orientation = orientation::bottom;
	};
private:
	parameters params;

	std::weak_ptr<wire_socket> primary;
	std::shared_ptr<wire_socket> slave;

public:
	wire_socket(const wire_socket&) = delete;
	wire_socket& operator=(const wire_socket&) = delete;

	wire_socket(wire_socket&&) = delete;
	wire_socket& operator=(wire_socket&&) = delete;

	~wire_socket() override = default;

public:
	struct all_parameters{
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
		parameters wire_socket_params;
	};

	wire_socket(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	wire_socket(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);
	
protected:
	std::shared_ptr<wire_socket> get_remote();
public:

	/**
	 * @brief Alignment of wire out.
	 */
	orientation get_orientation()const noexcept{
		return this->params.outlet_orientation;
	}

	void set_orientation(orientation o){
		this->params.outlet_orientation = o;
	}

	/**
	 * @brief Returns position and direction of the outlet.
	 * @return Array of two vectors. First is the position of the outlet within the widget.
	 *         Second is the unit vector of outlet wire direction.
	 */
	std::array<ruis::vector2, 2> outlet_pos()const noexcept;

	void connect(const std::shared_ptr<wire_socket>& o = nullptr);
	void disconnect();

	bool on_mouse_button(const ruis::mouse_button_event& event)override;

	void on_hovered_change(unsigned pointer_id) override;
	
	/**
	 * @brief Connection event callback.
	 * The notification method is called only on one of the wire sockets.
	 * @param to - the wire socket it is connected to.
	 */
	virtual void on_connected(wire_socket& to){}
	
	/**
	 * @brief Disconnection event notification.
	 * The notification method is called only on one of the wire sockets.
	 * @param from - the wire socket it was disconnected from.
	 */
	virtual void on_disconnected(wire_socket& from){}
};

namespace make{
inline utki::shared_ref<ruis::wire_socket> wire_socket(
	utki::shared_ref<ruis::context> context, //
	ruis::wire_socket::all_parameters params
)
{
	return utki::make_shared<ruis::wire_socket>(
		std::move(context), //
		std::move(params)
	);
}
}

}
