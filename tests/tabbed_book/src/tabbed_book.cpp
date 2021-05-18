#include "tabbed_book.hpp"

#include <morda/widgets/button/tab.hpp>
#include <morda/widgets/label/text.hpp>

using namespace morda;

tabbed_book::tabbed_book(std::shared_ptr<morda::context> context, const treeml::forest& desc) :
		morda::widget(std::move(context), desc),
		morda::column(
				this->context,
				treeml::read(R"(
					@tab_group{
						id { morda_tab_group }
						layout{
							dx { fill }
						}
					}
					@book{
						id { morda_book }
						layout {
							dx { fill }
							weight { 1 }
						}
					}
				)")
			),
		tab_group(this->get_widget_as<morda::tab_group>("morda_tab_group")),
		book(this->get_widget_as<morda::book>("morda_book"))
{}

void tabbed_book::add(const std::string& name, std::shared_ptr<morda::page> page){
	auto tab = this->context->inflater.inflate_as<morda::tab>(R"(
		@tab{
			@row{
				@text{
					id{text}
					text{cube}
				}
				@push_button{
					id{close_button}
					@image{
						layout{
							dx { 8dp }
							dy { 8dp }
						}
						image{morda_img_close}
					}
				}
			}
		}
	)");
	tab->get_widget_as<morda::text>("text").set_text(name);

	auto& close_btn = tab->get_widget_as<morda::push_button>("close_button");
	
	close_btn.click_handler = [page, tab_wp = utki::make_weak(tab)](morda::push_button& btn){
		auto tab = tab_wp.lock();
		ASSERT(tab)

		auto tab_parent = tab->parent();
		ASSERT(tab_parent)

		if(tab->is_pressed()){
			// find previous/next tab and activate it
			auto i = tab_parent->find(*tab);
			ASSERT(i != tab_parent->end())
			ASSERT(!tab_parent->empty())
			if(i == tab_parent->begin()){
				auto ni = std::next(i);
				if(ni != tab_parent->end()){
					auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
					ASSERT(next_tab)
					next_tab->set_pressed(true);
				}
			}else{
				ASSERT(i != tab_parent->begin())
				auto ni = std::prev(i);
				ASSERT(ni >= tab_parent->begin())
				auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
				ASSERT(next_tab)
				next_tab->set_pressed(true);
			}
		}
		tab->context->run_from_ui_thread([tab, page](){
			tab->remove_from_parent();
			LOG([](auto&o){o << "tear_out";})
			page->tear_out();
		});
	};
	this->tab_group.push_back(tab);
	
	this->book.push(page);

	tab->set_pressed(true);

	tab->press_handler = [page](morda::button& btn){
		if(btn.is_pressed()){
			page->go_to();
		}
	};
}
