#pragma once

#include <morda/widgets/widget.hpp>

class wire_socket : virtual public morda::widget{
	friend class wire_area;

	unsigned groupId;
public:
	enum class orientation{
		left,
		top,
		right,
		bottom
	};
private:
	orientation outlet_orientation = orientation::bottom;

	std::weak_ptr<wire_socket> primary;
	std::shared_ptr<wire_socket> slave;

public:
	wire_socket(const wire_socket&) = delete;
	wire_socket& operator=(const wire_socket&) = delete;

public:
	wire_socket(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
protected:
	std::shared_ptr<wire_socket> getRemote();
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
	std::array<morda::vector2, 2> outletPos()const noexcept;

	void connect(const std::shared_ptr<wire_socket>& o = nullptr);
	void disconnect();

	bool on_mouse_button(const morda::mouse_button_event& event)override;

	void on_hover_change(unsigned pointerID) override;
	
	/**
	 * @brief Connection event callback.
	 * The notification method is called only on one of the wire sockets.
	 * @param to - the wire socket it is connected to.
	 */
	virtual void onConnected(wire_socket& to){}
	
	/**
	 * @brief Disconnection event notification.
	 * The notification method is called only on one of the wire sockets.
	 * @param from - the wire socket it was disconnected from.
	 */
	virtual void onDisconnected(wire_socket& from){}
};
