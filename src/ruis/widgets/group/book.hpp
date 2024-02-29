/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#pragma once

#include "../../container.hpp"

namespace ruis {

class book;

class page : public virtual widget
{
	friend class book;

protected:
	page(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	book* parent_book = nullptr;

public:
	page(const page&) = delete;
	page& operator=(const page&) = delete;

	page(page&&) = delete;
	page& operator=(page&&) = delete;

	~page() override = default;

	book* get_parent_book()
	{
		return this->parent_book;
	}

	const book* get_parent_book() const
	{
		return this->parent_book;
	}

	void activate();

	bool is_active() const;

	virtual void on_show() {}

	virtual void on_hide() noexcept {}

	virtual void on_tear_out() noexcept {}

	utki::shared_ref<page> tear_out();
};

class book :
	public virtual widget, //
	private container
{
	friend class page;

	size_t active_page_index = std::numeric_limits<size_t>::max(); // invalid index

	std::vector<utki::shared_ref<page>> pages;

public:
	book(utki::shared_ref<ruis::context> context, widget::parameters widget_params);

	book(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	book(const book&) = delete;
	book& operator=(const book&) = delete;

	book(book&&) = delete;
	book& operator=(book&&) = delete;

	void push(const utki::shared_ref<page>& page);

	size_t size() const
	{
		return this->pages.size();
	}

	size_t get_active_index() const
	{
		return this->active_page_index;
	}

	const page* get_active_page() const;

	void activate(size_t page_index);
	void activate(const page& p);

	~book() override;

	std::function<void(book&)> active_page_change_handler;

	// page is either pushed or teared out
	std::function<void(book&, const page&)> pages_change_handler;

private:
	utki::shared_ref<page> tear_out(page& page);

	void notify_pages_change(const page& p);
};

namespace make {
inline utki::shared_ref<ruis::book> book(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params
)
{
	return utki::make_shared<ruis::book>(std::move(context), std::move(widget_params));
}
} // namespace make

} // namespace ruis
