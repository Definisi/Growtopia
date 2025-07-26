#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>
#include "enums.hpp"

/**
 * @file logger.hpp
 * @brief Thread-safe logging system for the Growtopia Bot application
 * @author Growtopia Bot Team
 */

namespace GrowtopiaBot {
    /**
     * @class Logger
     * @brief Thread-safe singleton logger class
     */
    class Logger {
    private:
        static std::unique_ptr<Logger> instance_;
        static std::mutex mutex_;
        
        std::ofstream log_file_;
        LogLevel min_level_;
        bool console_output_;
        std::mutex log_mutex_;
        
        Logger() = default;
        
        /**
         * @brief Get current timestamp as string
         * @return Formatted timestamp string
         */
        std::string get_timestamp() const;
        
        /**
         * @brief Convert LogLevel to string
         * @param level The log level to convert
         * @return String representation of log level
         */
        const char* level_to_string(LogLevel level) const;
        
    public:
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        
        /**
         * @brief Get singleton instance of Logger
         * @return Reference to Logger instance
         */
        static Logger& get_instance();
        
        /**
         * @brief Initialize logger with file output
         * @param log_file_path Path to log file
         * @param min_level Minimum log level to output
         * @param console_output Whether to also output to console
         * @return True if initialization successful
         */
        bool initialize(const std::string& log_file_path, 
                       LogLevel min_level = LogLevel::INFO, 
                       bool console_output = true);
        
        /**
         * @brief Log a message with specified level
         * @param level Log level
         * @param message Message to log
         * @param file Source file name (optional)
         * @param line Source line number (optional)
         */
        void log(LogLevel level, const std::string& message, 
                const char* file = nullptr, int line = -1);
        
        /**
         * @brief Log debug message
         * @param message Message to log
         */
        void debug(const std::string& message);
        
        /**
         * @brief Log info message
         * @param message Message to log
         */
        void info(const std::string& message);
        
        /**
         * @brief Log warning message
         * @param message Message to log
         */
        void warning(const std::string& message);
        
        /**
         * @brief Log error message
         * @param message Message to log
         */
        void error(const std::string& message);
        
        /**
         * @brief Log critical message
         * @param message Message to log
         */
        void critical(const std::string& message);
        
        /**
         * @brief Set minimum log level
         * @param level New minimum log level
         */
        void set_min_level(LogLevel level);
        
        /**
         * @brief Enable or disable console output
         * @param enable Whether to enable console output
         */
        void set_console_output(bool enable);
        
        /**
         * @brief Flush log buffers
         */
        void flush();
        
        ~Logger();
    };
}

// Convenience macros for logging with file and line information
#define LOG_DEBUG(msg) GrowtopiaBot::Logger::get_instance().log(GrowtopiaBot::LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) GrowtopiaBot::Logger::get_instance().log(GrowtopiaBot::LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) GrowtopiaBot::Logger::get_instance().log(GrowtopiaBot::LogLevel::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) GrowtopiaBot::Logger::get_instance().log(GrowtopiaBot::LogLevel::ERROR, msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) GrowtopiaBot::Logger::get_instance().log(GrowtopiaBot::LogLevel::CRITICAL, msg, __FILE__, __LINE__)