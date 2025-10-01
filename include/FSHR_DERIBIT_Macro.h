#pragma once

#define RULE_OF_FIVE_NONMOVABLE(NAME)               \
    NAME(const NAME&) = delete;                     \
    NAME(NAME&&) noexcept = delete;                 \
    NAME& operator=(const NAME&) = delete;          \
    NAME& operator=(NAME&&) noexcept = delete;      \
    virtual ~NAME() noexcept = default

#define RULE_OF_FIVE_MOVABLE(NAME)                  \
    NAME(const NAME&) = delete;                     \
    NAME(NAME&&) noexcept = default;                \
    NAME& operator=(const NAME&) = delete;          \
    NAME& operator=(NAME&&) noexcept = default;     \
    virtual ~NAME() noexcept = default

#define RULE_OF_FIVE_COPYABLE(NAME)                 \
    NAME(const NAME&) = default;                    \
    NAME(NAME&&) noexcept = default;                \
    NAME& operator=(const NAME&) = default;         \
    NAME& operator=(NAME&&) noexcept = default;     \
    virtual ~NAME() noexcept = default

#define RULE_OF_FIVE_TRIVIALLY_COPYABLE(NAME)       \
    NAME(const NAME&) = default;                    \
    NAME(NAME&&) noexcept = default;                \
    NAME& operator=(const NAME&) = default;         \
    NAME& operator=(NAME&&) noexcept = default;     \
    ~NAME() noexcept = default

