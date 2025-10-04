#pragma once

#include <ruisapp/application.hpp>

class application : public ruisapp::application
{
public:
	ruisapp::window& window;

	application();

	static application& inst()
	{
		return static_cast<application&>(ruisapp::inst());
	}
};
