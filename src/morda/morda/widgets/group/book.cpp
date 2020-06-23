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
	
	auto b = utki::make_shared_from_this(*this);
	
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
		b->push_back(pg);
		pg->on_show();
	});
}

void book::tear_out(page& pg)noexcept{
	ASSERT(&pg.parent_book() == this)
	
	auto i = std::find_if(this->pages.begin(), this->pages.end(), [&pg](const decltype(this->pages)::value_type& v) -> bool {
		return v.get() == &pg;
	});

	ASSERT(i != this->pages.end())

	bool is_active_page = std::distance(this->pages.begin(), i) == ssize_t(this->active_page_index);
	
	this->pages.erase(i);

	if(this->active_page_index != 0){
		--this->active_page_index;
	}

	if(is_active_page){
		pg.on_tear_out();
		this->clear();
		auto p = std::dynamic_pointer_cast<page>(this->pages[this->active_page_index]);
		this->push_back(p);
		if(p){
			p->on_show();
		}
	}
}

book::~book()noexcept{
	for(auto& p : this->pages){
		p->on_tear_out();
	}
}

book& page::parent_book(){
	if(!this->parent()){
		throw std::logic_error("page: the page is not yet shown, i.e. not added to any book");
	}
	auto p = dynamic_cast<book*>(this->parent());
	if(!p){
		TRACE(<< "parent id = " << this->parent()->id << std::endl)
		throw std::logic_error("parent_book(): page parent container is not a book");
	}
	
	return *p;
}

page::page(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{}

void page::tear_out()noexcept{
	this->context->run_from_ui_thread([this](){
		this->parent_book().tear_out(*this);
	});
}

