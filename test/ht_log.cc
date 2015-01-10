#include "ht_log.h"

silinan::Log g_log;

namespace silinan
{
	Log::Log()
	{
		to_std_ = false;
	}

	Log::Log(std::string& szFile)
	{
		Init(szFile);
	}

	Log::~Log()
	{
		Clear();
	}

	bool Log::Init(std::string& szFile, bool tostd)
	{
		Clear();

        file_.open(szFile.c_str());
		file_.fill('0');
		to_std_ = tostd;

        return true;
	}

	void Log::Clear()
	{
		if(file_.is_open()){
			file_.flush();
			file_.clear();
			file_.close();
		}
	}

	void Log::SetSTD(bool tostd)
	{
		to_std_ = tostd;
	}

	void Log::_prefix()
	{
		assert(file_.is_open());

        time_t t = ::time(NULL);
		tm* aTm = localtime(&t);
		//       YYYY   year
		//       MM     month (2 digits 01-12)
		//       DD     day (2 digits 01-31)
		//       HH     hour (2 digits 00-23)
		//       MM     minutes (2 digits 00-59)
		//       SS     seconds (2 digits 00-59)
        file_ << aTm->tm_year + 1900 << "-";
        file_.width(2);
        file_ << aTm->tm_mon + 1;
        file_ << "-";
        file_.width(2);
		file_ << aTm->tm_mday;
        file_ << " ";
        file_.width(2);
		file_ << aTm->tm_hour << ":";
        file_.width(2);
        file_ << aTm->tm_min << ":";
        file_.width(2);
        file_ << aTm->tm_sec << " ";
	}
}