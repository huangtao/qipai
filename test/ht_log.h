/*
htlogh

Copyright (c) 2013 huangtao(huangtao117@gmail.com)

功能:日志头文件
 
输出格式
 日期 时间  级别  具体信息
 如:2010-12-10 15:30:35  ERROR  write file failed.
*/
#ifndef _HTLOG_H
#define _HTLOG_H

#include <string>
#include <fstream>

// 对服务器而言有时性能是首要因素
// 注释此定义不加互斥锁
// 必须为每个需要写日志的服务器线程配置日志
#define HTLOG_MT

#ifdef HTLOG_MT
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#endif

// 简洁模式(时间+log)
//#define HTLOG_SIMP

namespace silinan
{
	class Log
	{
	public:
		Log();
		Log(std::string& szFile);
		~Log();

		bool Init(std::string& szFile, bool tostd = false);
		void Clear();
		void SetSTD(bool tostd);

		// 使日志类支持<<操作符
		template<typename T>
		Log& operator<<(const T& t)
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(mutex_);
#endif
			file_ << t;
			if(to_std_)
				std::cout << t;
			return *this;
		}

		Log& operator<<(Log& (*op)())
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(mutex_);
#endif
			return (*op)();
        }

        Log& operator<<(std::ostream& (*op)(std::ostream&))
        {
            (*op)(file_);
			if(to_std_)
				(*op)(std::cout);
            return *this;
        }

		Log& Otime()
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(this->mutex_);
#endif
			this->_prefix();
			return *this;
		}

#ifndef HTLOG_SIMP
        // 日志格式统一的前缀，使用自定义操控器完成
		Log& Warn()
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(this->mutex_);
#endif
			this->_prefix();
			this->file_ << "WARN ";
			return *this;
		}

		Log& Error()
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(this->mutex_);
#endif
			this->_prefix();
			this->file_ << "ERROR ";
			return *this;
		}

		Log& Info()
		{
#ifdef HTLOG_MT
			boost::mutex::scoped_lock lock(this->mutex_);
#endif
			this->_prefix();
			this->file_ << "INFO ";
			return *this;
		}
#endif

	protected:
		void _prefix();
	protected:
#ifdef HTLOG_MT
		boost::mutex		mutex_;
#endif
		std::ofstream		file_;
		bool				to_std_;
	};
}

extern silinan::Log	g_log;

#ifndef HTLOG_SIMP
#define TLOG g_log
#else
#define TLOG g_log.Otime()
#endif

#endif