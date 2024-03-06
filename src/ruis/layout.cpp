#include "layout.hpp"

#include "layouts/linear_layout.hpp"
#include "layouts/pile_layout.hpp"
#include "layouts/size_layout.hpp"
#include "layouts/trivial_layout.hpp"

using namespace ruis;

const utki::shared_ref<layout> layout::trivial = utki::make_shared<trivial_layout>();
const utki::shared_ref<layout> layout::size = utki::make_shared<size_layout>();
const utki::shared_ref<layout> layout::pile = utki::make_shared<pile_layout>();
const utki::shared_ref<layout> layout::row = utki::make_shared<linear_layout>(false);
const utki::shared_ref<layout> layout::column = utki::make_shared<linear_layout>(true);
