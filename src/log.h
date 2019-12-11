#ifndef __LOG_H__
#define __LOG_H__

#include "config.h"

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <vector>
#include <stdarg.h>

enum LogLevel{
    Error,     // 错误信息
    Warn,      // 警告信息，不影响程序执行
    Info,      // 业务信息
    Debug,     // 调试信息
    Trace      // 详细信息
};

extern const std::string default_file;

class Logger{
public:
	explicit Logger(
			bool log = false,
			LogLevel level = LogLevel::Error, 
			std::string file = default_file, 
			bool sync = true)
		:_log(log),
		_level(level), 
		_file(file), 
		_sync(sync) {
			loadConfig();
			if(_file == default_file){
				_file = generateLogFile();
			}
			if(_log){
				if(!fileOpen(_file, std::ios::out | std::ios::trunc)){
					std::cout<<"open log file error";
				}
			}
		}
	~Logger(){}

	void loadConfig();

	// 设置日志记录文件名
	//bool setLogFile(std::string &file){ _file = file; }
	// 获取日志记录文件名
	std::string logFile() const { return _file; }
	// 设置日志记录等级
	//bool setLogLevel(const LogLevel &level){ _level = level; }
	// 获取日志记录等级
	LogLevel logLevel()const { return _level; }

	// 写日志
	template<typename... Aargs>
		bool log(const LogLevel level, const char *file, const int line, const Aargs&... args){
			if(_log){
				std::string str_file = file;
				std::string str = str_file + " "+ std::to_string(line) + " ";
				logExpand(str, args...);
				return logWrite(level, str);
			}else
				return false;
		}

	// 日志初始化
	//bool logInit(const string &file, const bool sync_flag);
	// 日志关闭
	bool logClose();

private:
	bool fileOpen(const std::string &file, std::ios::openmode mode);

	bool fileWrite(const std::string &msg);
	bool fileWrite(const std::vector<std::string> &msg);
	
	bool logWrite(const LogLevel level, const std::string &msg);

	// 获取当前系统时间
	std::string currentTime() const;
	std::string currentTime(const std::string &format, const bool) const;

	// 生成日志文件名
	std::string generateLogFile() const;

private:
	template<typename T>
		void logExpand(std::string &str, const T &t){
			str += t;
		}
	template<typename T, typename... Tn>
		void logExpand(std::string &str, const T &t, const Tn &... args){
			std::string tmp(t);
			str += (tmp + " ");
			logExpand(str, args...);
		}

private:
	bool _log;			// 日志启动标志
	LogLevel _level;	// 日志等级
	std::mutex _mutex;	// 日志记录锁互斥量
	std::string _file;	// 日志记录文件名
	std::ofstream _ofs; // 日志记录文件流
	bool _sync;				// 日志同步标志，true: 同步写入；false:异步写入

	const int _log_length = 1024;	// 日志buffer最大长度
	std::vector<std::string> _log_vec;
	int _log_vec_len;
};

extern Logger g_log;
#define Log(level, ...) g_log.log(level, __FILE__, __LINE__, __VA_ARGS__)

#endif // __LOG_H__
