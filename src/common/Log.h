#pragma once

// Copyright 2019 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <sstream>

namespace rhi
{
    enum class LogSeverity 
    {
        Debug,
        Info,
        Warning,
        Error,
    };

    class LogMessage 
    {
    public:
        explicit LogMessage(LogSeverity severity);
        ~LogMessage();

        LogMessage(LogMessage&& other);
        LogMessage& operator=(LogMessage&& other);

        template <typename T>
        LogMessage& operator<<(T&& value) {
            mStream << value;
            return *this;
        }

    private:
        LogMessage(const LogMessage& other) = delete;
        LogMessage& operator=(const LogMessage& other) = delete;

        LogSeverity mSeverity;
        std::ostringstream mStream;
    };

    // Short-hands to create a LogMessage with the respective severity.
    LogMessage DebugLog();
    LogMessage InfoLog();
    LogMessage WarningLog();
    LogMessage ErrorLog();

    // DAWN_DEBUG is a helper macro that creates a DebugLog and outputs file/line/function
    // information
    LogMessage DebugLog(const char* file, const char* function, int line);
    #define DEBUG() DebugLog(__FILE__, __func__, __LINE__)
}