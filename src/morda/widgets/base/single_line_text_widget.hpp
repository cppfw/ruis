#pragma once

#include "text_widget.hpp"

namespace morda{

class single_line_text_widget : public text_widget{
	mutable morda::rectangle bb;

	std::u32string text;
protected:
	vector2 measure(const morda::vector2& quotum)const noexcept override;

	single_line_text_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	const morda::rectangle& get_bounding_box()const{
		return this->bb;
	}

	void recompute_bounding_box(){
		this->bb = this->get_font().get().get_bounding_box(this->get_text());
	}
public:
	using text_widget::set_text;

	void set_text(std::u32string&& text)override;

	std::u32string get_text()const override;

	void on_font_change()override{
		this->recompute_bounding_box();
	}

	void on_text_change()override;
};

}
