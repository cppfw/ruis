#pragma once

#include "../group/Pile.hpp"

namespace morda{

/**
 * @brief Container to be used for intercepting keyboard key events.
 * From GUI scripts it can be instantiated as "KeyProxy".
 */
class KeyProxy : public Pile{
public:
	KeyProxy(const stob::Node* chain = nullptr) :
			Widget(chain),
			Pile(chain)
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
