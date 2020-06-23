#include "../../../src/morda/morda/widgets/button/push_button.hpp"

#include <mordavokne/application.hpp>

#include "main_page.hpp"
#include "cube_page.hpp"

main_page::main_page(std::shared_ptr<morda::context> c) :
		widget(std::move(c), puu::forest()),
		page(this->context, puu::forest()),
		pile(this->context, puu::read(*mordavokne::inst().get_res_file("res/main_page.gui")))
{
	auto& b = this->get_widget_as<morda::push_button>("main_button");
	b.click_handler = [this](morda::push_button& b){
		this->parent_book().push(std::make_shared<cube_page>(this->context));
	};
}
