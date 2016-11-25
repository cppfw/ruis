#pragma once

#include <utki/Unique.hpp>

namespace morda{


class Shader_n : public utki::Unique{
public:
	Shader_n();
	
	Shader_n(const Shader_n&) = delete;
	Shader_n& operator=(const Shader_n&) = delete;
	
	virtual ~Shader_n()noexcept{}
	
private:

};

}
