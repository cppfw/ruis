#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include <morda/widgets/button/tab_group.hpp>
#include <morda/widgets/group/book.hpp>
#include <morda/widgets/group/tabbed_book.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
tst::set set("tabbed_book", [](tst::suite& suite){

	// This test checks that tabbed_book class convey's virtual methods
	// from it's parent class, even though it is inherited via private inheritance.
	suite.add("tabbed_book_has_access_to_base_class_render_method", []{
		auto c = make_dummy_context();

		// make initializations needed by morda::tabbed_book
		c->loader.mount_res_pack(papki::fs_file("../../res/morda_res/main.res"));
		c->inflater.register_widget<morda::tab_group>("tab_group");
		c->inflater.register_widget<morda::book>("book");

		class sub_tabbed_book : public morda::tabbed_book{
		public:
			sub_tabbed_book(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
					morda::widget(c, desc),
					tabbed_book(this->context, desc)
			{}
		} stb(std::move(c), treeml::forest());

		morda::matrix4 m;
		stb.render(m);
		// TODO: call oser methods like on_key(), on_mouse_move(), etc.
	});
});
}
