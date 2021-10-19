/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "book.hpp"

#include "../../context.hpp"

using namespace morda;

book::book(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		pile(nullptr, treeml::forest())
{}

void book::push(std::shared_ptr<page> pg){
	if(!pg){
		throw std::logic_error("book::push(): tried to push nullptr");
	}

	if(pg->parent_book){
		if(pg->parent_book == this){
			throw std::logic_error("book::push(): the page is already in this book");
		}
		throw std::logic_error("book::push(): the page is already in some book");
	}
	
	auto& lp = this->get_layout_params(*pg);
	lp.dims.set(widget::layout_params::fill);
	
	pg->parent_book = this;
	this->pages.push_back(pg);

	this->notify_pages_change(*pg);

	this->context->run_from_ui_thread([
			bk = utki::make_shared_from(*this),
			index = this->pages.size() - 1
		]()
	{
		bk->activate(index);
	});
}

void book::tear_out(page& pg)noexcept{
	ASSERT(&pg.get_parent_book() == this)
	
	auto i = std::find_if(
			this->pages.begin(),
			this->pages.end(),
			[&pg](const auto& v) -> bool {
				return v.get() == &pg;
			}
		);

	ASSERT(i != this->pages.end())

	auto index = size_t(std::distance(this->pages.begin(), i));

	bool is_active_page = index == this->active_page_index;
	
	this->pages.erase(i);
	pg.parent_book = nullptr;

	if(index <= this->active_page_index){
		if(this->active_page_index == 0){
			this->active_page_index = std::numeric_limits<size_t>::max(); // invalid
		}else{
			--this->active_page_index;
		}
	}

	if(is_active_page){
		pg.on_hide();
		this->clear();
		
		if(!this->pages.empty()){
			ASSERT(this->active_page_index < this->pages.size())
			auto p = std::dynamic_pointer_cast<page>(this->pages[this->active_page_index]);
			this->push_back(p);
			if(p){
				p->on_show();
			}
		}
	}

	this->notify_pages_change(pg);
}

void book::notify_pages_change(const page& p){
	if(this->pages_change_handler){
		this->pages_change_handler(*this, p);
	}
}

void book::activate(const page& p){
	if(&p.get_parent_book() != this){
		throw std::logic_error("book::go_to(): requested page is not in this book");
	}

	auto i = std::find_if(
			this->pages.begin(),
			this->pages.end(),
			[&p](auto pg){
				return pg.get() == &p;
			}
		);
	ASSERT(i != this->pages.end())

	this->activate(std::distance(this->pages.begin(), i));
}

void book::activate(size_t page_number){
	if(page_number >= this->pages.size()){
		throw std::logic_error("book::go_to(): requested page number is out of scope");
	}

	if(this->pages.empty()){
		return;
	}

	if(this->active_page_index == page_number){
		// page is already active
		return;
	}

	// the this->active_page_index can be invalid in case the first page was just pushed (see push() method implementation)
	if(this->active_page_index < this->pages.size()){
		this->pages[this->active_page_index]->on_hide();
	}

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

const book& page::get_parent_book()const{
	if(!this->parent_book){
		throw std::logic_error("page::get_parent_book(): page is not in a book");
	}
	return *this->parent_book;
}

page::page(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc)
{}

void page::tear_out()noexcept{
	this->context->run_from_ui_thread([this](){
		this->get_parent_book().tear_out(*this);
	});
}

void page::activate(){
	this->get_parent_book().activate(*this);
}
