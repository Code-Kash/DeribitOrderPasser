#pragma once

#include "FSHR_DERIBIT_Enums.h"
#include "FSHR_DERIBIT_Utils.h"
#include "FSHR_DERIBIT_Macro.h"
#include "FSHR_DERIBIT_ProtocolTraits.h"

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace fischer::deribit
{
    template<typename Traits = DeribitTraits>
    class Logger
    {
    public:
        RULE_OF_FIVE_NONMOVABLE(Logger);

        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }

        void Initialize(const std::string& logFile = "",
                       LogLevel minLevel = LogLevel::Info,
                       bool enableConsole = true,
                       bool enableFile = true)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_MinLogLevel = minLevel;
            m_EnableConsole = enableConsole;
            m_EnableFile = enableFile;

            if (true == m_EnableFile && false == logFile.empty())
            {
                if (m_FileStream.is_open())
                {
                    m_FileStream.close();
                }
                m_FileStream.open(logFile, std::ios::out | std::ios::app);
                m_LogFilePath = logFile;
            }
        }

        void Shutdown()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (true == m_FileStream.is_open())
            {
                m_FileStream.flush();
                m_FileStream.close();
            }
        }

        template<typename... Args>
        void Debug(const Args&... args)
        {
            Log(LogLevel::Debug, args...);
        }

        template<typename... Args>
        void Info(const Args&... args)
        {
            Log(LogLevel::Info, args...);
        }

        template<typename... Args>
        void Warning(const Args&... args)
        {
            Log(LogLevel::Warning, args...);
        }

        template<typename... Args>
        void Error(const Args&... args)
        {
            Log(LogLevel::Error, args...);
        }

        template<typename... Args>
        void Critical(const Args&... args)
        {
            Log(LogLevel::Critical, args...);
        }

        void SetLogLevel(LogLevel level)
        {
            m_MinLogLevel.store(level);
        }

        LogLevel GetLogLevel() const
        {
            return m_MinLogLevel.load();
        }

        void SetConsoleOutput(bool enable)
        {
            m_EnableConsole.store(enable);
        }

        void SetFileOutput(bool enable)
        {
            m_EnableFile.store(enable);
        }

    private:
        // Private constructor for singleton
        Logger()
            : m_MinLogLevel{LogLevel::Info}
            , m_EnableConsole{Traits::EnableConsoleLogging}
            , m_EnableFile{Traits::EnableFileLogging}
        {
        }

        template<typename... Args>
        void Log(LogLevel level, const Args&... args)
        {
            if (level < m_MinLogLevel.load())
            {
                return;
            }

            std::ostringstream oss;
            oss.str().reserve(Traits::MaxLogMessageLength);

            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

            oss << " [" << utils::LogLevelToString(level) << "] ";

            ((oss << args << ' '), ...);

            std::string message = oss.str();

            if (false == message.empty() && ' ' == message.back())
            {
                message.pop_back();
            }

            std::lock_guard<std::mutex> lock(m_Mutex);

            if (true == m_EnableConsole.load())
            {
                if (LogLevel::Error == level || LogLevel::Critical == level)
                {
                    std::cerr << message << std::endl;
                }
                else
                {
                    std::cout << message << std::endl;
                }
            }

            if (true == m_EnableFile.load() && true == m_FileStream.is_open())
            {
                m_FileStream << message << std::endl;
                m_FileStream.flush();
            }
        }

    private:
        mutable std::mutex m_Mutex;
        std::ofstream m_FileStream;
        std::string m_LogFilePath;
        std::atomic<LogLevel> m_MinLogLevel;
        std::atomic<bool> m_EnableConsole;
        std::atomic<bool> m_EnableFile;
    };

    #define LOG_DEBUG(...)   fischer::deribit::Logger<>::GetInstance().Debug(__VA_ARGS__)
    #define LOG_INFO(...)    fischer::deribit::Logger<>::GetInstance().Info(__VA_ARGS__)
    #define LOG_WARNING(...) fischer::deribit::Logger<>::GetInstance().Warning(__VA_ARGS__)
    #define LOG_ERROR(...)   fischer::deribit::Logger<>::GetInstance().Error(__VA_ARGS__)
    #define LOG_CRITICAL(...) fischer::deribit::Logger<>::GetInstance().Critical(__VA_ARGS__)
}