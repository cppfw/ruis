#pragma once

#include "../widget.hpp"

namespace morda{

class ratio_proxy : virtual public widget{
    real x_above_y = 1;
public:
    ratio_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc);

    ratio_proxy(const ratio_proxy&) = delete;
    ratio_proxy& operator=(const ratio_proxy&) = delete;

    void set_aspect_ratio(real x_above_y);

    real get_aspect_ratio()const noexcept{
        return this->x_above_y;
    }

    morda::vector2 measure(const morda::vector2& quotum)const override;
};

}
