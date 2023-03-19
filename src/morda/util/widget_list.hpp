#pragma once

#include <vector>

#include <utki/shared_ref.hpp>

namespace morda{

class widget;

using widget_list = std::vector<utki::shared_ref<widget>>;
using semiconst_widget_list = const std::vector<utki::shared_ref<widget>>;
using const_widget_list = const std::vector<utki::shared_ref<const widget>>;
static_assert(sizeof(widget_list) == sizeof(const_widget_list), "sizeof(widget_list) differs from sizeof(const_widget_list)");
static_assert(sizeof(widget_list) == sizeof(semiconst_widget_list), "sizeof(widget_list) differs from sizeof(semiconst_widget_list)");
static_assert(sizeof(widget_list::value_type) == sizeof(const_widget_list::value_type), "sizeof(widget_list::value_type) differs from sizeof(const_widget_list::value_type)");
static_assert(sizeof(widget_list::value_type) == sizeof(semiconst_widget_list::value_type), "sizeof(widget_list::value_type) differs from sizeof(semiconst_widget_list::value_type)");

}
