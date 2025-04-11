#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include <ruis/widget/button/tab_group.hpp>
#include <ruis/widget/group/book.hpp>
#include <ruis/widget/group/tabbed_book.hpp>
#include <ruis/gui.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
const tst::set set("tabbed_book", [](tst::suite& suite){

	// This test checks that tabbed_book class conveys virtual methods
	// from its parent class, even though it is inherited via private inheritance.
	suite.add("tabbed_book_has_access_to_base_class_render_method", []{
		auto c = make_dummy_context();

		ruis::gui gui(c);

		// make initializations needed by ruis::tabbed_book
		c.get().loader().mount_res_pack(papki::fs_file("../../res/ruis_res/main.res"));

		class sub_tabbed_book : public ruis::tabbed_book{
		public:
			sub_tabbed_book(utki::shared_ref<ruis::context> context) :
				ruis::widget(std::move(context), {}, {}),
				tabbed_book(this->context, ruis::tabbed_book::all_parameters{}, {})
			{}
		};

		auto book = utki::make_shared<sub_tabbed_book>(c);

		ruis::matrix4 m;
		book.get().render(m);
		// TODO: call other methods like on_key(), on_mouse_move(), etc.
	});
});
}
