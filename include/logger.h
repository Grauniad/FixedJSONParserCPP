
#ifndef LOGGER
#define LOGGER

#include <iostream>

enum LOG_LEVEL {
    LOG_WARNING = 0,
    LOG_DEFAULT,
    LOG_OVERVIEW,
    LOG_VERBOSE,
    LOG_VERY_VERBOSE,
    __NUM_LOG_LEVELS
};

#define MINIMUM_LOGGING_LEVEL LOG_OVERVIEW

#define SLOG_FROM(level, context, text) \
    if (level < MINIMUM_LOGGING_LEVEL) \
    { \
        std::cout << text; \
    }

#define LOG_FROM(level, context, text) \
    if (level < MINIMUM_LOGGING_LEVEL) \
    { \
        std::cout << text; \
    }

#endif // include guard
