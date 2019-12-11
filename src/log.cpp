#include "log.h"
#include <chrono>
#include <stdio.h>
#include <sstream>
#include <iomanip>

const std::string default_file = "./run.log"; // 默认日志文件名

Logger g_log(LogLevel::Info);

static std::string logLeveltoString(const LogLevel &level){
	std::string level_str = "";
	switch(level){
		case LogLevel::Error:
			level_str = "Error ";
			break;
		case LogLevel::Warn:
			level_str = "Warn ";
			break;
		case LogLevel::Info:
			level_str = "Info ";
			break;
		case LogLevel::Debug:
			level_str = "Debug ";
			break;
		case LogLevel::Trace:
			level_str = "Trace ";
			break;
		default:
			break;
	}
	return level_str;
}

void Logger::loadConfig(){
	std::string log;
	Config config(g_config_file);
	log = config.Read("log",log);
	_log = (std::stoi(log) == 0) ? false : true;
}

// 描述：
//      将msg信息写入日志文件
// 参数：
//      msg     I   日志信息
// 返回值：
//      false   写入日志失败
//      true    写入日志成功
bool Logger::logWrite(const LogLevel level, const std::string &msg){
	if(_level >= level){
		std::unique_lock<std::mutex> lck(_mutex);
		if(_sync){
			if(!fileWrite( currentTime() + logLeveltoString(level) + msg)){
				return false;
			}
		}else{
			_log_vec_len += msg.length();
			_log_vec.push_back(currentTime() + logLeveltoString(level) + msg);
			if(_log_vec_len > _log_length){
				if(!fileWrite(_log_vec)){
					return false;
				}
				_log_vec.clear();
				_log_vec_len = 0;
			}
		}
	}
	return true;
}

// 描述：
//      打开文件
// 参数：
//      file    I   打开文件文件名
//      mode    I   打开文件模式
// 返回值：
//      false   打开文件失败
//      true    打开文件成功
bool Logger::fileOpen(const std::string &file, std::ios::openmode mode){
	_ofs.open(file, mode);
	if(!_ofs.is_open()){
		return false;
	}
	return true;
}

// 描述：
//      关闭文件
// 返回值：
//      true    关闭文件成功
bool Logger::logClose(){
	std::unique_lock<std::mutex> lck(_mutex);
	if(_ofs.is_open()){
		if(!_log_vec.empty()){
			fileWrite(_log_vec);
			_log_vec.clear();
			_log_vec_len = 0;
		}
		_ofs.close();
	}
	return true;
}

bool Logger::fileWrite(const std::string &msg){
	if(_ofs.is_open()){
		_ofs << msg <<std::endl;
	}else{
		return false;
	}
	return true;
}

bool Logger::fileWrite(const std::vector<std::string> &msg){
	for(auto &e : msg){
		if(!fileWrite(e))
			return false;
	}
	return true;
}

std::string Logger::currentTime() const{
	std::string format = "%Y-%m-%d %H:%M:%S";
	return currentTime(format, true);
}

std::string Logger::currentTime(const std::string &format, const bool type) const{
	auto now = std::chrono::system_clock::now();
	auto now_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_t), format.c_str());
	std::string now_str = ss.str();
	if(type){
		auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		return now_str + "." + std::to_string(mill - now_t*1000) + " ";
	}else{
		return now_str;
	}
}

std::string Logger::generateLogFile() const{
	std::string format = "%Y%m%d%H%M%S";
	std::string file;
	file += currentTime(format, false);
	file += ".log";
	return file;
}
