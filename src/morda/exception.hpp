#pragma once


#include <utki/Exc.hpp>


namespace morda{

/**
 * @brief Basic morda exception class.
 */
class exception : public utki::Exc{
public:
	exception(const std::string& message) :
			utki::Exc(message)
	{}
};

//TODO: deprecated, remove.
/**
 * @brief Basic morda exception class.
 */
class Exc : public exception{
public:
	Exc(const std::string& message) :
			exception(message)
	{}
};

}
