#pragma once

#include "../group/pile.hpp"

namespace morda{

/**
 * @brief container to be used for intercepting keyboard key events.
 * From GUI scripts it can be instantiated as "KeyProxy".
 */
class KeyProxy : public pile{
public:
	KeyProxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			pile(this->context, desc)
	{}
	
	KeyProxy(const KeyProxy&) = delete;
	KeyProxy& operator=(const KeyProxy&) = delete;
	
	/**
	 * @brief Keyboard key signal.
	 * Emitted when a keyboard key event reaches this widget.
	 */
	std::function<bool(bool isDown, morda::key keyCode)> key;
	
	virtual bool on_key(bool isDown, morda::key keyCode)override;
};

}
