#pragma once

#include "App.hpp"

#include <utki/config.hpp>


namespace mordavokne{



/**
 * @brief Create application instance.
 * User needs to define this factory function to create his application instance.
 * @return New application instance.
 */
#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif

std::unique_ptr<App> createApp(int argc, const char** argv, const utki::Buf<std::uint8_t> savedState);



}
