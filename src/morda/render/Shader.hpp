#pragma once

#include <utki/Unique.hpp>

namespace morda{


class Shader : public utki::Unique{
public:
	Shader();
	
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	
	virtual ~Shader()noexcept{}
	
private:

};

}
