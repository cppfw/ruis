#pragma once

#include "pile.hpp"

namespace morda{

class book;

class page : public virtual widget{
	friend class book;
protected:
	page(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	page(const page&) = delete;
	page& operator=(const page&) = delete;
public:
	book& parent_book();

	virtual void on_show(){}
	virtual void on_hide(){}
	virtual void on_close()noexcept{}
	
	void close()noexcept;
private:

};

class book :
		public virtual widget,
		public pile
{
	friend class page;
	
	size_t active_page_index = 0;
public:
	book(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	book(const book&) = delete;
	book& operator=(const book&) = delete;
	
	void push(std::shared_ptr<page> page);
	
	~book()noexcept;

	// void render(const matrix4& matrix)const override{
	// 	this->pile::render(matrix);
	// }
private:

	void close(page& page)noexcept;
};

}
