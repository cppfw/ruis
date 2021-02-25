#pragma once

#include "../group/pile.hpp"

namespace morda{

/**
 * @brief container to be used for intercepting keyboard key events.
 * From GUI scripts it can be instantiated as "key_proxy".
 */
class key_proxy : public pile{
public:
	key_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			pile(this->context, desc)
	{}
	
	key_proxy(const key_proxy&) = delete;
	key_proxy& operator=(const key_proxy&) = delete;
	
	/**
	 * @brief Keyboard key signal.
	 * Emitted when a keyboard key event reaches this widget.
	 */
	std::function<bool(key_proxy& w, bool is_down, morda::key key_code)> key_handler;
	
	virtual bool on_key(bool is_down, morda::key key_code)override;
};

}
