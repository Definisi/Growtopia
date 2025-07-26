#include "logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>

/**
 * @file logger.cpp
 * @brief Implementation of the Logger class
 * @author Growtopia Bot Team
 */

namespace GrowtopiaBot {
    std::unique_ptr<Logger> Logger::instance_ = nullptr;
    std::mutex Logger::mutex_;
    
    Logger& Logger::get_instance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::unique_ptr<Logger>(new Logger());
        }
        return *instance_;
    }
    
    bool Logger::initialize(const std::string& log_file_path, LogLevel min_level, bool console_output) {
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        // Create directory if it doesn't exist
        std::filesystem::path file_path(log_file_path);
        if (file_path.has_parent_path()) {
            std::filesystem::create_directories(file_path.parent_path());
        }
        
        log_file_.open(log_file_path, std::ios::app);
        if (!log_file_.is_open()) {
            return false;
        }
        
        min_level_ = min_level;
        console_output_ = console_output;
        
        info("Logger initialized successfully");
        return true;
    }
    
    std::string Logger::get_timestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    const char* Logger::level_to_string(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    void Logger::log(LogLevel level, const std::string& message, const char* file, int line) {
        if (level < min_level_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        std::stringstream log_entry;
        log_entry << "[" << get_timestamp() << "] ";
        log_entry << "[" << level_to_string(level) << "] ";
        
        if (file && line > 0) {
            std::filesystem::path file_path(file);
            log_entry << "[" << file_path.filename().string() << ":" << line << "] ";
        }
        
        log_entry << message;
        
        // Write to file
        if (log_file_.is_open()) {
            log_file_ << log_entry.str() << std::endl;
            log_file_.flush();
        }
        
        // Write to console
        if (console_output_) {
            if (level >= LogLevel::ERROR) {
                std::cerr << log_entry.str() << std::endl;
            } else {
                std::cout << log_entry.str() << std::endl;
            }
        }
    }
    
    void Logger::debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }
    
    void Logger::info(const std::string& message) {
        log(LogLevel::INFO, message);
    }
    
    void Logger::warning(const std::string& message) {
        log(LogLevel::WARNING, message);
    }
    
    void Logger::error(const std::string& message) {
        log(LogLevel::ERROR, message);
    }
    
    void Logger::critical(const std::string& message) {
        log(LogLevel::CRITICAL, message);
    }
    
    void Logger::set_min_level(LogLevel level) {
        std::lock_guard<std::mutex> lock(log_mutex_);
        min_level_ = level;
    }
    
    void Logger::set_console_output(bool enable) {
        std::lock_guard<std::mutex> lock(log_mutex_);
        console_output_ = enable;
    }
    
    void Logger::flush() {
        std::lock_guard<std::mutex> lock(log_mutex_);
        if (log_file_.is_open()) {
            log_file_.flush();
        }
    }
    
    Logger::~Logger() {
        if (log_file_.is_open()) {
            info("Logger shutting down");
            log_file_.close();
        }
    }
}