/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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
#include "../../layouts/pile_layout.hpp"

using namespace morda;

book::book(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	container(this->context, treeml::forest(), pile_layout::instance)
{}

void book::push(const utki::shared_ref<page>& pg)
{
	if (pg.get().parent_book) {
		if (pg.get().parent_book == this) {
			throw std::logic_error("book::push(): the page is already in this book");
		}
		throw std::logic_error("book::push(): the page is already in some book");
	}

	auto& lp = pg.get().get_layout_params();
	lp.dims.set(layout_params::fill);

	pg.get().parent_book = this;
	this->pages.push_back(pg);

	this->notify_pages_change(pg.get());

	this->context.get().run_from_ui_thread([bk = utki::make_shared_from(*this), index = this->pages.size() - 1]() {
		bk.get().activate(index);
	});
}

utki::shared_ref<page> book::tear_out(page& pg) noexcept
{
	// book::tear_out() is private, hense ASSERT instead of if(){throw}
	ASSERT(pg.get_parent_book() == this)

	auto i = std::find_if(this->pages.begin(), this->pages.end(), [&pg](const auto& v) -> bool {
		return &v.get() == &pg;
	});

	ASSERT(i != this->pages.end())

	auto index = size_t(std::distance(this->pages.begin(), i));

	bool is_active_page = index == this->active_page_index;

	auto ret = *i;

	this->pages.erase(i);
	pg.parent_book = nullptr;

	if (index <= this->active_page_index) {
		if (this->active_page_index == 0) {
			if (this->pages.empty()) {
				this->active_page_index = std::numeric_limits<size_t>::max(); // invalid
			} else {
				// active page index remains 0
			}
		} else {
			--this->active_page_index;
		}
	}

	if (is_active_page) {
		pg.on_hide();
		this->clear();

		if (!this->pages.empty()) {
			ASSERT(this->active_page_index < this->pages.size(), [this](auto& o) {
				o << "this->active_page_index = " << this->active_page_index
				  << ", this->pages.size() = " << this->pages.size();
			})
			auto p = this->pages[this->active_page_index];
			this->push_back(p);
			p.get().on_show();
		}
	}

	pg.on_tear_out();

	this->notify_pages_change(pg);

	return ret;
}

void book::notify_pages_change(const page& p)
{
	if (this->pages_change_handler) {
		this->pages_change_handler(*this, p);
	}
}

void book::activate(const page& p)
{
	if (p.get_parent_book() != this) {
		throw std::logic_error("book::activate(): requested page is not in this book");
	}

	auto i = std::find_if(this->pages.begin(), this->pages.end(), [&p](const auto& pg) {
		return &pg.get() == &p;
	});
	ASSERT(i != this->pages.end())

	this->activate(std::distance(this->pages.begin(), i));
}

void book::activate(size_t page_number)
{
	if (page_number >= this->pages.size()) {
		throw std::logic_error("book::activate(): requested page number is out of scope");
	}

	if (this->pages.empty()) {
		return;
	}

	if (this->active_page_index == page_number) {
		// page is already active
		return;
	}

	// the this->active_page_index can be invalid in case the first page was just pushed (see push() method
	// implementation)
	if (this->active_page_index < this->pages.size()) {
		this->pages[this->active_page_index].get().on_hide();
	}

	this->clear();

	this->active_page_index = page_number;
	this->push_back(this->pages[this->active_page_index]);
	this->pages[this->active_page_index].get().on_show();

	if (this->active_page_change_handler) {
		this->active_page_change_handler(*this);
	}
}

book::~book() noexcept
{
	if (this->active_page_index >= this->pages.size()) {
		return;
	}

	this->pages[this->active_page_index].get().on_hide();
}

const page* book::get_active_page() const
{
	if (this->active_page_index < this->pages.size()) {
		return &this->pages[this->active_page_index].get();
	}
	return nullptr;
}

page::page(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc)
{}

utki::shared_ref<page> page::tear_out() noexcept
{
	if (!this->get_parent_book()) {
		return utki::make_shared_from(*this);
	}
	return this->get_parent_book()->tear_out(*this);
}

void page::activate()
{
	if (!this->get_parent_book()) {
		return;
	}
	this->get_parent_book()->activate(*this);
}

bool page::is_active() const
{
	if (!this->parent_book) {
		return false;
	}

	return this == this->parent_book->get_active_page();
}
