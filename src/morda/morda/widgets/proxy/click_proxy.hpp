#pragma once

#include "../widget.hpp"

namespace morda{

class click_proxy : virtual public widget{
    bool is_pressed = false;
public:
    click_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc);

	click_proxy(const click_proxy&) = delete;
	click_proxy& operator=(const click_proxy&) = delete;

    bool on_mouse_button(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id)override;
    void on_hover_changed(unsigned pointer_id)override;

    /**
     * @brief Handler for mouse press state changes.
     */
    std::function<bool(widget& w, bool is_pressed)> press_handler;
    
    /**
     * @brief Handler for clicked event.
     */
    std::function<bool(widget& w)> click_handler;
};

}
