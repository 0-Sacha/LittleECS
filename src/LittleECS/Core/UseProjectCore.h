#pragma once

#include "ProjectCore/FMT/FMT.h"

#ifdef LECS_LOGGER_ENABLE
    #include "ProjectCore/LoggerManager/LoggerManager.h"

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

    #define LECS_TRACE(...)     LECS::Core::Logger().Trace(__VA_ARGS__)
    #define LECS_INFO(...)      LECS::Core::Logger().Info(__VA_ARGS__)
    #define LECS_WARN(...)      LECS::Core::Logger().Warn(__VA_ARGS__)
    #define LECS_ERROR(...)     LECS::Core::Logger().Error(__VA_ARGS__)
    #define LECS_FATAL(...)     LECS::Core::Logger().Fatal(__VA_ARGS__)
#endif
