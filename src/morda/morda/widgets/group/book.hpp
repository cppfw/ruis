#pragma once

#include "pile.hpp"

namespace morda{

class book;

class page : public virtual widget{
	friend class book;
protected:
	page(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	page(const page&) = delete;
	page& operator=(const page&) = delete;

	book* parent_book = nullptr;
public:
	book& get_parent_book();
	const book& get_parent_book()const;

	void go_to();

	virtual void on_show(){}
	virtual void on_hide()noexcept{}
	
	void tear_out()noexcept;
private:

};

class book :
		public virtual widget,
		private pile
{
	friend class page;
	
	size_t active_page_index = std::numeric_limits<size_t>::max(); // invalid index

	std::vector<std::shared_ptr<page>> pages;
public:
	book(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	book(const book&) = delete;
	book& operator=(const book&) = delete;
	
	void push(std::shared_ptr<page> page);
	
	size_t size()const{
		return this->pages.size();
	}

	size_t get_active_page()const{
		return this->active_page_index;
	}

	void go_to(size_t page_number);
	void go_to(const page& p);

	~book()noexcept;

	// page is either pushed or teared out
	std::function<void(book&, const page&)> pages_change_handler;
private:
	void tear_out(page& page)noexcept;

	void notify_pages_change(const page& p);
};

}
