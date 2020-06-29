#ifndef LOG_H
#define LOG_H
#include <memory>
#include <spdlog/spdlog.h>

class Log
{
public:
	Log();
	~Log();

	static void initialize();

	inline static std::shared_ptr<spdlog::logger>& getLogger() { return p_logger; }

private:
	static std::shared_ptr<spdlog::logger> p_logger;

};

#ifdef _DEBUG
#define logTrace(...)    ::Log::getLogger()->trace(__VA_ARGS__)
#define logInfo(...)     ::Log::getLogger()->info(__VA_ARGS__)
#define logWarning(...)  ::Log::getLogger()->warn(__VA_ARGS__)
#define logError(...)    ::Log::getLogger()->error(__VA_ARGS__)
#define logFatal(...)    ::Log::getLogger()->fatal(__VA_ARGS__)
#else
#define logTrace
#define logInfo
#define logWarning
#define logError
#define logFatal
#endif
#endif