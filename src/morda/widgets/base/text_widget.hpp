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

#pragma once

#include "../../widget.hpp"
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
	utki::shared_ref<const res::font> font;

public:
	text_widget(const text_widget&) = delete;
	text_widget& operator=(const text_widget&) = delete;

	void set_font(const utki::shared_ref<const res::font>& font);

	const res::font& get_font()const{
		return this->font.get();
	}

	void set_text(const std::string& text){
		this->set_text(utki::to_utf32(text));
	}

	virtual void set_text(std::u32string&& text) = 0;

	virtual std::u32string get_text()const = 0;

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
	text_widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

private:

};

}
