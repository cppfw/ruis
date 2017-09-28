#pragma once

#include "Button.hpp"


namespace morda{

/**
 * @brief Basic class of a toggle button.
 * Toggle button is a button which can be checked and unchecked.
 * In GUI script it has the 'checked{true/false}' attribute which can set initial
 * checked state of the widget.
 */
class ToggleButton : public Button{
	bool isChecked_v;
	
protected:	
	void onPressedChanged() override;
	
	/**
	 * @brief Invoked when button checked state changes.
	 * Default implementation emits 'checkedChanged' signal.
	 */
	virtual void onCheckedChanged();
public:
	ToggleButton(const stob::Node* chain = nullptr);
	
	/**
	 * @brief Set checked state.
	 * If checked state changes as a result of this function call it will also
	 * invoke onCheckedChanged().
	 * @param checked - whether the new state should be set to checked (true) or unchecked (false).
	 */
	void setChecked(bool checked);
	
	/**
	 * @brief Change checked state to opposite.
	 */
	void toggle(){
		this->setChecked(!this->isChecked());
	}
	
	/**
	 * @brief Tell if the button is checked or not.
	 * @return true if the button is checked.
	 * @return false otherwise.
	 */
	bool isChecked()const noexcept{
		return this->isChecked_v;
	}
	
	/**
	 * @brief Checked changed signal.
	 * Emitted when checked state changes.
	 */
	std::function<void(ToggleButton&)> checkedChanged;
};

}
