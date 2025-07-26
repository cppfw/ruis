#pragma once

#include <ruis/widget/widget.hpp>

utki::shared_ref<ruis::widget> make_table_list_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
);
