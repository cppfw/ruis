#pragma once

#include "widget.hpp"

namespace ruis::make{

struct widget_params{
    ruis::layout_params lp;
};

utki::shared_ref<ruis::widget> widget(
    const widget_params& params
);

}
