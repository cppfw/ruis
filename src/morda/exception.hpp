#pragma once


#include <utki/exception.hpp>


namespace morda{

/**
 * @brief Basic morda exception class.
 */
class exception : public utki::exception{
public:
	exception(const std::string& message) :
			utki::exception(message)
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
