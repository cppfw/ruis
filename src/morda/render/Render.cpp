#include "Render.hpp"

#include <ting/debug.hpp>

#include "../config.hpp"



#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL || M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES

#include "Render_OpenGL.cpp"

#elif M_MORDA_RENDER == M_MORDA_RENDER_DIRECTX

#include "Render_DirectX.cpp"

#else
#	error "unknown render API"
#endif
