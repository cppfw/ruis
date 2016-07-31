#pragma once


#include <utki/Exc.hpp>



namespace morda{

/**
 * @brief Basic morda exception class.
 */
class Exc : public utki::Exc{
public:
	Exc(const std::string& message) :
			utki::Exc(message)
	{}
};

/**
 * @brief Exception class indicating illegal value.
 */
class IllegalValueExc : public Exc{
public:
	IllegalValueExc(const std::string& message) :
			Exc(message)
	{}
};


}
