#pragma once

#include "ProjectCore/FMT/FMT.h"
#include "ProjectCore/LoggerManager/LoggerManager.h"

#include <utility>

namespace LittleECS
{
    class Core
    {
    public:
        static ProjectCore::LoggerManager::BasicLogger& Logger() { return m_Logger; }
    
    private:
        static inline ProjectCore::LoggerManager::BasicLogger m_Logger{};
    };
}

#define LECS_LOGGER_ENABLE

#ifdef LECS_LOGGER_ENABLE
#define LECS_TRACE(...)	    LittleECS::Core::Logger().Trace(__VA_ARGS__)
#define LECS_INFO(...)	    LittleECS::Core::Logger().Info(__VA_ARGS__)
#define LECS_WARN(...)	    LittleECS::Core::Logger().Warn(__VA_ARGS__)
#define LECS_ERROR(...)	    LittleECS::Core::Logger().Error(__VA_ARGS__)
#define LECS_FATAL(...)	    LittleECS::Core::Logger().Fatal(__VA_ARGS__)
#else
#define LECS_TRACE(...)
#define LECS_INFO(...)
#define LECS_WARN(...)
#define LECS_ERROR(...)
#define LECS_FATAL(...)
#endif


#define LECS_ASSERT(x, ...) if (!(x)) { LECS_FATAL("Assert FAILED! : {}", #x); __debugbreak(); }
