#include "book.hpp"

#include "../../context.hpp"

using namespace morda;

book::book(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		pile(nullptr, puu::forest())
{}

void book::push(std::shared_ptr<page> pg){
	if(!pg){
		throw std::logic_error("book: tried to push nullptr");
	}
	
	auto& lp = this->get_layout_params(*pg);
	lp.dims.set(widget::layout_params::fill);
	
	auto b = utki::make_shared_from(*this);
	
	this->context->run_from_ui_thread([b, pg](){
		if(!b->pages.empty()){
			ASSERT(b->children().size() == 1)
			ASSERT(b->active_page_index < b->pages.size())
			auto p = dynamic_cast<page*>(b->pages[b->active_page_index].get());
			if(p){
				p->on_hide();
			}
			b->clear();
		}

		b->active_page_index = b->pages.size();
		b->pages.push_back(pg);
		pg->parent_book = b.get();
		b->push_back(pg);
		pg->on_show();
	});
}

void book::tear_out(page& pg)noexcept{
	ASSERT(&pg.get_parent_book() == this)
	
	auto i = std::find_if(this->pages.begin(), this->pages.end(), [&pg](const decltype(this->pages)::value_type& v) -> bool {
		return v.get() == &pg;
	});

	ASSERT(i != this->pages.end())

	bool is_active_page = size_t(std::distance(this->pages.begin(), i)) == this->active_page_index;
	
	this->pages.erase(i);
	pg.parent_book = nullptr;

	if(this->active_page_index != 0){
		--this->active_page_index;
	}

	if(is_active_page){
		pg.on_hide();
		this->clear();
		auto p = std::dynamic_pointer_cast<page>(this->pages[this->active_page_index]);
		this->push_back(p);
		if(p){
			p->on_show();
		}
	}
}

void book::go_to(size_t page_number){
	if(page_number >= this->pages.size()){
		throw std::logic_error("book::go_to(): requested page number is out of scope");
	}

	if(this->pages.empty()){
		return;
	}

	ASSERT(this->active_page_index < this->pages.size())

	this->pages[this->active_page_index]->on_hide();

	this->clear();

	this->active_page_index = page_number;
	this->push_back(this->pages[this->active_page_index]);
	this->pages[this->active_page_index]->on_show();
}

book::~book()noexcept{
	if(this->active_page_index >= this->pages.size()){
		return;
	}

	this->pages[this->active_page_index]->on_hide();
}

book& page::get_parent_book(){
	if(!this->parent_book){
		throw std::logic_error("page::get_parent_book(): page is not in a book");
	}
	return *this->parent_book;
}

page::page(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{}

void page::tear_out()noexcept{
	this->context->run_from_ui_thread([this](){
		this->get_parent_book().tear_out(*this);
	});
}
