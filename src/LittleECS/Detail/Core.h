#pragma once

#include "ProjectCore/FMT/FMT.h"
#include "ProjectCore/LoggerManager/LoggerManager.h"

#include <utility>

namespace LECS
{
    class Core
    {
    public:
        static ProjectCore::LoggerManager::BasicLogger& Logger() { return m_Logger; }
    
    private:
        static inline ProjectCore::LoggerManager::BasicLogger m_Logger{};
    };
}

#ifdef LECS_DEBUG
    #define LECS_LOGGER_ENABLE
    #define LECS_ASSERT_ENABLE
#endif


#ifdef LECS_ASSERT_ENABLE
    #define LECS_LOGGER_ENABLE
#endif

#ifdef LECS_LOGGER_ENABLE
    #define LECS_TRACE(...)	    LECS::Core::Logger().Trace(__VA_ARGS__)
    #define LECS_INFO(...)	    LECS::Core::Logger().Info(__VA_ARGS__)
    #define LECS_WARN(...)	    LECS::Core::Logger().Warn(__VA_ARGS__)
    #define LECS_ERROR(...)	    LECS::Core::Logger().Error(__VA_ARGS__)
    #define LECS_FATAL(...)	    LECS::Core::Logger().Fatal(__VA_ARGS__)
#else
    #define LECS_TRACE(...)
    #define LECS_INFO(...)
    #define LECS_WARN(...)
    #define LECS_ERROR(...)
    #define LECS_FATAL(...)
#endif


#ifdef LECS_ASSERT_ENABLE
    #define LECS_ASSERT(x, ...) if (!(x)) { LECS_FATAL("Assert FAILED! : {} " #x __VA_ARGS__); __debugbreak(); }
#else
    #define LECS_ASSERT(x, ...)
#endif
