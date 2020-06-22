#pragma once

#include <morda/widgets/group/pile.hpp>

namespace morda{

class book;

class page : public virtual widget{
protected:
	page(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	page(const page&) = delete;
	page& operator=(const page&) = delete;
	
	book& parent_book();
public:
	virtual void on_show(){}
	virtual void on_hide(){}
	virtual void on_close()noexcept{}
	
	void close()noexcept;
private:

};

class book :
		private pile,
		public virtual widget
{
	friend class page;
	
	std::vector<std::shared_ptr<page>> pages;
public:
	book(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	book(const book&) = delete;
	book& operator=(const book&) = delete;
	
	void push(std::shared_ptr<page> page);
	
	~book()noexcept;
private:

	void close(page& page)noexcept;
};

}
