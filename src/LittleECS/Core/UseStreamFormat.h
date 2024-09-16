#pragma once

#include "StreamFormat/FMT.h"

#ifdef LECS_LOGGER_ENABLE
    #include "StreamFormat/FLog.h"

    namespace LECS
    {
        class Core
        {
        public:
            static StreamFormat::FLog::BasicLogger& Logger() { return m_Logger; }
        private:
            static inline StreamFormat::FLog::BasicLogger m_Logger{};
        };
    }

    #define LECS_TRACE(...)     LECS::Core::Logger().Trace(__VA_ARGS__)
    #define LECS_INFO(...)      LECS::Core::Logger().Info(__VA_ARGS__)
    #define LECS_WARN(...)      LECS::Core::Logger().Warn(__VA_ARGS__)
    #define LECS_ERROR(...)     LECS::Core::Logger().Error(__VA_ARGS__)
    #define LECS_FATAL(...)     LECS::Core::Logger().Fatal(__VA_ARGS__)
#endif
