#include "log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Donut {

	std::shared_ptr<spdlog::logger> Log::sptr_core_logger_;
	std::shared_ptr<spdlog::logger> Log::sptr_client_logger_;

	void Log::init()
	{
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Donut.log", true));

		//log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
		//log_sinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S:%f %z] [%5t] [source %g] [function %!] [line %#] [%^%=5l%$] %v");
		//log_sinks[1]->set_pattern("[%T] [%l] %n: [src %g] [func %!] [line %#] [%^%=5l%$] %v");

		log_sinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S:%f %z] [%5t] [function %!] [line %#] [%^%=5l%$] %v");
		log_sinks[1]->set_pattern("[%T] [%l] %n: [src %g] [func %!] [line %#] [%^%=5l%$] %v");

		sptr_core_logger_ = std::make_shared<spdlog::logger>("DONUT CORE", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(sptr_core_logger_);
		sptr_core_logger_->set_level(spdlog::level::trace);
		sptr_core_logger_->flush_on(spdlog::level::trace);

		sptr_client_logger_ = std::make_shared<spdlog::logger>("DONUT CLIENT", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(sptr_client_logger_);
		sptr_client_logger_->set_level(spdlog::level::trace);
		sptr_client_logger_->flush_on(spdlog::level::trace);
	}

}