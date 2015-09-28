/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <utki/Exc.hpp>



namespace morda{

class Exc : public utki::Exc{
public:
	Exc(const std::string& message) :
			utki::Exc(message)
	{}
};

class IllegalValueExc : public Exc{
public:
	IllegalValueExc(const std::string& message) :
			Exc(message)
	{}
};


}//~namespace
