#include "make.hpp"

using namespace ruis::make;

utki::shared_ref<ruis::widget> ruis::make::widget(
    const widget_params& params
){
    return utki::make_shared<ruis::widget>();
}