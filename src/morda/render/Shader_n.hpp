#pragma once

#include <utki/Unique.hpp>

namespace morda{


class Shader_n : public utki::Unique{
public:
	Shader_n();
	
	Shader_n(const Shader_n&) = delete;
	Shader_n& operator=(const Shader_n&) = delete;
	
	virtual ~Shader_n()noexcept{}
	
	
	/**
	 * @brief Vertex data rendering mode.
	 * Enumeration defining how to interpret vertex data when rendering.
	 */
	enum class Mode_e{
		//NOTE: do not change order!!!
		
		TRIANGLES,
		TRIANGLE_FAN,
		LINE_LOOP,
		TRIANGLE_STRIP
	};
	
private:

};

}
