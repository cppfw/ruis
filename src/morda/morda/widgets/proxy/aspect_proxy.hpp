#pragma once

#include "../widget.hpp"

namespace morda{

class aspect_proxy : widget{
    real x_above_y = 1;
public:
    aspect_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc);

    aspect_proxy(const aspect_proxy&) = delete;
    aspect_proxy& operator=(const aspect_proxy&) = delete;

    void set_aspect_ratio(real x_above_y);

    real get_aspect_ratio()const noexcept{
        return this->x_above_y;
    }

    morda::vector2 measure(const morda::vector2& quotum)const override;
};

}
