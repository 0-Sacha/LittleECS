#pragma once

#include "CompilerInfo.h"

#include <utility>

#ifdef LECS_DEBUG
    #define LECS_LOGGER_ENABLE
    #define LECS_ASSERT_ENABLE
#endif

#ifdef LECS_ASSERT_ENABLE
    #define LECS_LOGGER_ENABLE
#endif

#ifdef LECS_USE_PROJECTCORE
    #include "UseProjectCore.h"
#endif

#ifdef LECS_LOGGER_ENABLE
    #define LECS_TRACE(...)     // TODO: ?
    #define LECS_INFO(...)      // TODO: ?
    #define LECS_WARN(...)      // TODO: ?
    #define LECS_ERROR(...)     // TODO: ?
    #define LECS_FATAL(...)     // TODO: ?
#else
    #define LECS_TRACE(...)
    #define LECS_INFO(...)
    #define LECS_WARN(...)
    #define LECS_ERROR(...)
    #define LECS_FATAL(...)
#endif

#ifdef LECS_ASSERT_ENABLE
    #ifdef LECS_COMPILER_MSVC
        #define LECS_ASSERT(x, ...)    if(!(x)) { LECS_FATAL("ASSERT FAILED! : " #x __VA_ARGS__); __debugbreak(); }
    #else
        #include <csignal>
        #define LECS_ASSERT(x, ...)    if(!(x)) { LECS_FATAL("ASSERT FAILED! : " #x __VA_ARGS__); std::raise(SIGINT); }
    #endif
#else
    #define LECS_ASSERT(...)
#endif
