#pragma once

#include "../widget.hpp"

namespace morda{

class click_proxy : virtual public widget{
    bool is_pressed_ = false;
    bool deferred_release_ret;
public:
    click_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc);

	click_proxy(const click_proxy&) = delete;
	click_proxy& operator=(const click_proxy&) = delete;

    bool on_mouse_button(const mouse_button_event& event)override;
    void on_hover_change(unsigned pointer_id)override;

    bool is_pressed()const noexcept{
        return this->is_pressed_;
    }

    /**
     * @brief Handler for mouse press state changes.
     */
    std::function<bool(click_proxy& w)> press_change_handler;
    
    /**
     * @brief Handler for clicked event.
     */
    std::function<void(click_proxy& w)> click_handler;
};

}
