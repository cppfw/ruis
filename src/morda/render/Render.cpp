#include "Render.hpp"

#include <utki/debug.hpp>

#include "../config.hpp"



#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL || M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES

#include "Render_OpenGL.inc"

#elif M_MORDA_RENDER == M_MORDA_RENDER_DIRECTX

#include "Render_DirectX.inc"

#else
#	error "unknown render API"
#endif
