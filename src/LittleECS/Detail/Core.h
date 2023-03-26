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

#ifdef LECS_LOGGER_ENABLE
#define LECS_TRACE(...)	ProjectCore::LoggerManager::BasicLogger::GetCoreInstance().Trace(__VA_ARGS__)
#define LECS_INFO(...)	ProjectCore::LoggerManager::BasicLogger::GetCoreInstance().Info(__VA_ARGS__)
#define LECS_WARN(...)	ProjectCore::LoggerManager::BasicLogger::GetCoreInstance().Warn(__VA_ARGS__)
#define LECS_ERROR(...)	ProjectCore::LoggerManager::BasicLogger::GetCoreInstance().Error(__VA_ARGS__)
#define LECS_FATAL(...)	ProjectCore::LoggerManager::BasicLogger::GetCoreInstance().Fatal(__VA_ARGS__)
#else
#define LECS_TRACE(...)
#define LECS_INFO(...)
#define LECS_WARN(...)
#define LECS_ERROR(...)
#define LECS_FATAL(...)
#endif


#define LECS_ASSERT(x, ...) if (!(x)) { LECS_FATAL("Assert FAILED! : {}", #x); }
