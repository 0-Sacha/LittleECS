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

#ifdef LECS_DEBUG
    #define LECS_LOGGER_ENABLE
    #define LECS_ASSERT_ENABLE
#endif


#ifdef LECS_ASSERT_ENABLE
    #define LECS_LOGGER_ENABLE
#endif

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

#ifdef LECS_ASSERT_ENABLE
	#ifdef LECS_COMPILER_VS
		#define LECS_ASSERT(x, ...)	if(!(x)) { LECS_FATAL("ASSERT FAILED! : " #x __VA_ARGS__); __debugbreak(); }
	#else
		#include <csignal>
		#define LECS_ASSERT(x, ...)	if(!(x)) { LECS_FATAL("ASSERT FAILED! : " #x __VA_ARGS__); std::raise(SIGINT); }
	#endif
#else
	#define LECS_ASSERT(x)
#endif
