#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include <ruis/widgets/button/tab_group.hpp>
#include <ruis/widgets/group/book.hpp>
#include <ruis/widgets/group/tabbed_book.hpp>
#include <ruis/gui.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
const tst::set set("tabbed_book", [](tst::suite& suite){

	// This test checks that tabbed_book class convey's virtual methods
	// from it's parent class, even though it is inherited via private inheritance.
	suite.add("tabbed_book_has_access_to_base_class_render_method", []{
		auto c = make_dummy_context();

		ruis::gui gui(c);

		// make initializations needed by ruis::tabbed_book
		c.get().loader.mount_res_pack(papki::fs_file("../../res/morda_res/main.res"));
		c.get().inflater.register_widget<ruis::tab_group>("tab_group");

		class sub_tabbed_book : public ruis::tabbed_book{
		public:
			sub_tabbed_book(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
					ruis::widget(c, desc),
					tabbed_book(this->context, desc)
			{}
		} stb(c, treeml::forest());

		ruis::matrix4 m;
		stb.render(m);
		// TODO: call other methods like on_key(), on_mouse_move(), etc.
	});
});
}
