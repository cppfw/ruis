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
		if(b->children().size() != 0){
			ASSERT(b->active_page_index < b->children().size())
			auto p = dynamic_cast<page*>(b->children()[b->active_page_index].get());
			if(p){
				p->on_hide();
			}
		}

		b->active_page_index = b->children().size();
		b->push_back(pg);
		pg->on_show();
	});
}

void book::close(page& pg)noexcept{
	ASSERT(&pg.parent_book() == this)
	
	auto i = this->find(pg);

	ASSERT(i != this->children().end())

	this->erase(i);

	if(std::distance(this->children().begin(), i) == ssize_t(this->active_page_index)){
		pg.on_close();
	}

	if(this->active_page_index != 0){
		--this->active_page_index;
	}

	ASSERT(this->active_page_index < this->children().size())

	auto p = std::dynamic_pointer_cast<page>(this->children()[this->active_page_index]);
	if(p){
		p->on_show();
	}
}

book::~book()noexcept{
	for(auto& c : this->children()){
		auto p = dynamic_cast<page*>(c.get());
		if(p){
			p->on_close();
		}
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

void page::close()noexcept{
	this->context->run_from_ui_thread([this](){
		this->parent_book().close(*this);
	});
}
