#pragma once

#include <ruis/widget/widget.hpp>

class wire_socket : virtual public ruis::widget{
	friend class wire_area;

	unsigned groupId = 0;
public:
	enum class orientation{
		left,
		top,
		right,
		bottom,

		enum_size
	};
private:
	orientation outlet_orientation = orientation::bottom;

	std::weak_ptr<wire_socket> primary;
	std::shared_ptr<wire_socket> slave;

public:
	wire_socket(const wire_socket&) = delete;
	wire_socket& operator=(const wire_socket&) = delete;

	wire_socket(wire_socket&&) = delete;
	wire_socket& operator=(wire_socket&&) = delete;

	~wire_socket() override = default;

public:
	wire_socket(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);
	
protected:
	std::shared_ptr<wire_socket> get_remote();
public:

	/**
	 * @brief Alignment of wire out.
	 */
	orientation get_orientation()const noexcept{
		return this->outlet_orientation;
	}

	void set_orientation(orientation o){
		this->outlet_orientation = o;
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
