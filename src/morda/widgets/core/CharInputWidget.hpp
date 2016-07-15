/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/Buf.hpp>

#include "Widget.hpp"

#include "../../util/keycodes.hpp"

namespace morda{


class CharInputWidget : virtual public Widget{

protected:
	CharInputWidget() :
			Widget(nullptr)
	{}

public:	
	/**
	 * @brief Handler for character input.
     * @param unicode - unicode string of entered characters in UTF-32.
	 * @param key - key code associated with character input, can be UNKNOWN.
     */
	virtual void onCharacterInput(const utki::Buf<std::uint32_t> unicode, Key_e key) = 0;
	
	virtual ~CharInputWidget()noexcept{}
};


}
