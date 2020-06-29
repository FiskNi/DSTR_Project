#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::p_logger;

Log::Log()
{

}

void Log::initialize()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	p_logger = spdlog::stdout_color_mt("console");
	p_logger->set_level(spdlog::level::trace);
}


Log::~Log()
{

}