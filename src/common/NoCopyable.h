#pragma once

class NonCopyable
{
public:
    constexpr NonCopyable() = default;
    ~NonCopyable() = default;

protected:
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;

private:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
};