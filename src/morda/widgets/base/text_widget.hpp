#pragma once

#include "../widget.hpp"
#include "color_widget.hpp"

#include "../../res/res_font.hpp"

#include <r4/rectangle.hpp>

#include <list>
#include <vector>

namespace morda{

/**
 * @brief Abstract widget displaying a text.
 */
//TODO: should not be color widget
class text_widget : public color_widget{
	std::shared_ptr<res_font> font_v;

protected:
	std::vector<std::u32string> lines;

public:
	text_widget(const text_widget&) = delete;
	text_widget& operator=(const text_widget&) = delete;

	void set_font(std::shared_ptr<res_font> font);

	const morda::font& font()const{
		return this->font_v->font();
	}

	void set_text(const std::string& text){
		this->set_text(unikod::toUtf32(text));
	}

	void set_text(std::u32string&& text);

	std::u32string get_text()const;

	void clear(){
		this->set_text(std::u32string());
	}

	virtual void on_font_changed(){}

	virtual void on_text_changed(){
		if (this->text_change_handler) {
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
	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;

	single_line_text_widget(std::shared_ptr<morda::context> c, const puu::forest& desc);

	const morda::rectangle& get_bounding_box()const{
		return this->bb;
	}

	void recompute_bounding_box(){
		this->bb = this->font().get_bounding_box(this->get_text());
	}
public:
	void on_font_changed()override{
		this->recompute_bounding_box();
	}

	void on_text_changed()override;
};

}
