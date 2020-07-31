#pragma once

#include "../widget.hpp"
#include "color_widget.hpp"

#include "../../res/font.hpp"

#include <r4/rectangle.hpp>

#include <list>
#include <vector>

namespace morda{

/**
 * @brief Abstract widget displaying a text.
 */
class text_widget : virtual public widget{
	std::shared_ptr<res::font> font_v;

protected:
	std::vector<std::u32string> lines;

public:
	text_widget(const text_widget&) = delete;
	text_widget& operator=(const text_widget&) = delete;

	void set_font(std::shared_ptr<res::font> font);

	const morda::font& get_font()const{
		return this->font_v->get_font();
	}

	void set_text(const std::string& text){
		this->set_text(utki::to_utf32(text));
	}

	void set_text(std::u32string&& text);

	std::u32string get_text()const;

	void clear(){
		this->set_text(std::u32string());
	}

	virtual void on_font_change(){}

	virtual void on_text_change(){
		if(this->text_change_handler){
			this->text_change_handler(*this);
		}
	}

	std::function<void(text_widget& w)> text_change_handler;

protected:
	text_widget(std::shared_ptr<morda::context> c, const puu::forest& desc);

private:

};

class single_line_text_widget : public text_widget{
	mutable morda::rectangle bb;

protected:
	vector2 measure(const morda::vector2& quotum)const noexcept override;

	single_line_text_widget(std::shared_ptr<morda::context> c, const puu::forest& desc);

	const morda::rectangle& get_bounding_box()const{
		return this->bb;
	}

	void recompute_bounding_box(){
		this->bb = this->get_font().get_bounding_box(this->get_text());
	}
public:
	void on_font_change()override{
		this->recompute_bounding_box();
	}

	void on_text_change()override;
};

}
