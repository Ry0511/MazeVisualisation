//
// Created by -Ry on 16/01/2023.
//

#ifndef MAZEVISUALISATION_LOGGING_H
#define MAZEVISUALISATION_LOGGING_H

#ifdef Debug

#include <format>
#include <cstdarg>
#include <iostream>
#include <chrono>

//############################################################################//
// | COMPILER SPECIFIC BREAKING |
//############################################################################//

#ifdef _MSC_VER
#define PANIC __debugbreak()
#else
#define PANIC throw -1
#endif

//############################################################################//
// | GET TIME |
//############################################################################//

namespace Chrono = std::chrono;
using Clock = std::chrono::system_clock;

static std::string get_time() {
    auto now = Clock::to_time_t(Clock::now());
    char buffer[16]{};

    tm local_time{};
    localtime_s(&local_time, &now);
    std::strftime(buffer, 16, "%T", &local_time);

    return std::string(buffer);
}

//############################################################################//
// | DEBUG MODE LOGGING |
//############################################################################//

#define HLOG(scope, header, msg, ...) std::clog << std::format("{:<10} {:<6} | {:<20} {}", get_time(), scope, header, std::format(msg, __VA_ARGS__)) << std::endl
#define LOG(header, msg, ...) std::clog << std::format("{:<10} {:<6} | {}\n", get_time(), header, std::format(msg, __VA_ARGS__))
#define INFO(msg, ...) LOG("INFO", msg, __VA_ARGS__)
#define HINFO(header, msg, ...) HLOG("INFO", header, msg, __VA_ARGS__)
#define WARN(msg, ...) LOG("WARN", msg, __VA_ARGS__)
#define HWARN(header, msg, ...) HLOG("WARN", header, msg, __VA_ARGS__)
#define ERR(msg, ...) LOG("ERROR", msg, __VA_ARGS__)
#define HERR(header, msg, ...) HLOG("ERROR", header, msg, __VA_ARGS__)

#define ASSERT(condition, msg, ...) if (!(condition)) [&](){ ERR(msg, __VA_ARGS__); PANIC; }()

//############################################################################//
// | RELEASE MODE LOGGING (N/A) |
//############################################################################//

#else

#define HLOG(scope, header, msg, ...)
#define LOG(header, msg, ...)
#define INFO(msg, ...)
#define HINFO(header, msg, ...)
#define WARN(msg, ...)
#define HWARN(header, msg, ...)
#define ERR(msg, ...)
#define HERR(header, msg, ...)

#endif

#endif //MAZEVISUALISATION_LOGGING_H
