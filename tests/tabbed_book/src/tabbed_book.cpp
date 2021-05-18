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
{
	// TODO: set book::pages_change_handler to remove the tab
}

void tabbed_book::add(const std::string& name, std::shared_ptr<morda::page> page){
	auto t = this->context->inflater.inflate_as<morda::tab>(R"(
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
	t->get_widget_as<morda::text>("text").set_text(name);

	auto& close_btn = t->get_widget_as<morda::push_button>("close_button");
	
	close_btn.click_handler = [
			tabbed_book_wp = utki::make_weak_from(*this),
			tab_wp = utki::make_weak(t)
		](morda::push_button& btn)
	{
		auto tb = tabbed_book_wp.lock();
		ASSERT(tb)

		auto t = tab_wp.lock();
		ASSERT(t)

		tb->tear_out(*t);

		// auto tab_parent = tab->parent();
		// ASSERT(tab_parent)

		// if(tab->is_pressed()){
		// 	// find previous/next tab and activate it
		// 	auto i = tab_parent->find(*tab);
		// 	ASSERT(i != tab_parent->end())
		// 	ASSERT(!tab_parent->empty())
		// 	if(i == tab_parent->begin()){
		// 		auto ni = std::next(i);
		// 		if(ni != tab_parent->end()){
		// 			auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
		// 			ASSERT(next_tab)
		// 			next_tab->set_pressed(true);
		// 		}
		// 	}else{
		// 		ASSERT(i != tab_parent->begin())
		// 		auto ni = std::prev(i);
		// 		ASSERT(ni >= tab_parent->begin())
		// 		auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
		// 		ASSERT(next_tab)
		// 		next_tab->set_pressed(true);
		// 	}
		// }
		// tab->context->run_from_ui_thread([tab, page](){
		// 	tab->remove_from_parent();
		// 	LOG([](auto&o){o << "tear_out";})
		// 	page->tear_out();
		// });
	};
	this->tab_group.push_back(t);
	
	this->book.push(page);

	t->set_pressed(true);

	t->press_handler = [page](morda::button& btn){
		if(btn.is_pressed()){
			page->go_to();
		}
	};

	this->tab_to_page_map.insert(std::make_pair(t.get(), page.get()));
}

std::shared_ptr<page> tabbed_book::tear_out(tab& t){
	auto i = this->tab_to_page_map.find(&t);
	if(i == this->tab_to_page_map.end()){
		throw std::logic_error("tabbed_book::tear_out(): tab not found");
	}

	auto pg = utki::make_shared_from(*i->second);

	this->tab_to_page_map.erase(i);

	ASSERT(t.parent() == &this->tab_group)

	// if the tab is currently active we need to switch to another tab before removing
	if(t.is_pressed()){
		// find previous/next tab and activate it
		auto i = this->tab_group.find(t);
		ASSERT(i != this->tab_group.end())
		ASSERT(!this->tab_group.empty())
		if(i == this->tab_group.begin()){
			auto ni = std::next(i);
			if(ni != this->tab_group.end()){
				auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
				ASSERT(next_tab)
				next_tab->set_pressed(true);
			}
		}else{
			ASSERT(i != this->tab_group.begin())
			auto ni = std::prev(i);
			ASSERT(ni >= this->tab_group.begin())
			auto next_tab = std::dynamic_pointer_cast<morda::tab>(*ni);
			ASSERT(next_tab)
			next_tab->set_pressed(true);
		}
	}

	// TODO: assert that page is not active
	// ASSERT(!pg->is_active())
	pg->tear_out();

	this->context->run_from_ui_thread([t = utki::make_shared_from(t)](){
		t->remove_from_parent();
	});

	return pg;
}
