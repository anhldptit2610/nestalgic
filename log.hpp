#ifndef LOG_HPP
#define LOG_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace NESTALGIC_LOG {
	void init() {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		auto logger = std::make_shared<spdlog::logger>("console", console_sink);
		logger->set_level(spdlog::level::trace);
		spdlog::set_default_logger(logger);
	}
	void log(const std::string& message) {
		spdlog::info(message);
	}
}

#endif