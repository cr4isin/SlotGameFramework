#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <map>
#include <thread>
#include <chrono>
#include <windows.h>
#include <format>
#include <ranges>
#include <array>
#include "ANSI.h"

// ----------------------
// Formatting Utilities
// ----------------------

// Python-style format shortcut: F("Value = {}", val)
template <typename... Args>
inline auto F(std::string_view fmt, Args&&... args) {
    return std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
}

// Fast format for high-performance use (e.g., map keys in tight loops)
// Returns a temporary std::string_view into a thread-local buffer — do not store it!
template <typename... Args>
inline std::string K(std::format_string<Args...> fmt, Args&&... args) {
    thread_local std::array<std::string, 8> buffers;
    thread_local int index = 0;

    std::string& temp = buffers[index];
    index = (index + 1) % buffers.size();

    temp.clear();
    std::format_to(std::back_inserter(temp), fmt, std::forward<Args>(args)...);

    // Return a copy to avoid lifetime issues with internal buffer
    return temp;
}

// Helper type trait to detect string-like types
template <typename T> struct is_string_like : std::false_type {};
template <typename CharT, typename Traits, typename Alloc>
struct is_string_like<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};
template <typename CharT, typename Traits>
struct is_string_like<std::basic_string_view<CharT, Traits>> : std::true_type {};
template <typename CharT, size_t N>
struct is_string_like<CharT[N]> : std::true_type {};
template <typename CharT>
struct is_string_like<CharT*> : std::true_type {};
template <typename T>
inline constexpr bool is_string_like_v = is_string_like<T>::value;

// Generic formatter for printing containers recursively
template <std::ranges::range R>
    requires (!is_string_like_v<R>)
struct std::formatter<R> : std::formatter<std::string> {
    auto format(const R& range, auto& ctx) const {
        std::string out = "[";
        bool first = true;
        for (const auto& elem : range) {
            if (!first) out += ", ";
            first = false;
            out += std::format("{}", elem);
        }
        out += "]";
        return std::formatter<std::string>::format(out, ctx);
    }
};

// ----------------------
// PrintTools Class
// ----------------------

class PrintTools {
public:
    // Prints a message and pauses until the user presses Enter. Optionally clears the line afterward.
    static void PauseMessage(std::string message, bool clearAfter = false) {
        std::string input;
        std::cout << message << std::flush;
        std::getline(std::cin, input);
        if (clearAfter) std::cout << ANSI::PreviousLine(1) << ANSI::ClearLine() << std::flush;
    }

    // Returns a timestamp in "YYYY-MM-DD HH:MM:SS" format.
    static std::string CurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &now);

        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%F %T", &localTime);
        return std::string(buffer);
    }

    // Formats a duration in milliseconds as "HH:MM:SS"
    static std::string Duration(long long milliseconds) {
        long long totalSeconds = milliseconds / 1000;
        long long hours = totalSeconds / 3600;
        long long minutes = (totalSeconds / 60) % 60;
        long long seconds = totalSeconds % 60;

        return F("{:02}:{:02}:{:02}", hours, minutes, seconds);
    }

    // Spawns a given number of new processes running the specified program, with optional delay in seconds.
    static void SpawnProcesses(std::string programName, int numTimes, int delayInSeconds = 0) {
        std::wstring fullCommandLine = L"\"" + std::wstring(programName.begin(), programName.end()) + L"\" 1";

        for (int i = 0; i < numTimes; ++i) {
            STARTUPINFO startupInfo;
            PROCESS_INFORMATION processInfo;

            ZeroMemory(&startupInfo, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);
            ZeroMemory(&processInfo, sizeof(processInfo));

            if (!CreateProcess(
                NULL,
                const_cast<LPWSTR>(fullCommandLine.c_str()),
                NULL,
                NULL,
                FALSE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &startupInfo,
                &processInfo
            )) {
                std::cerr << "Error creating process. Exiting." << std::endl;
                return;
            }

            std::this_thread::sleep_for(std::chrono::seconds(delayInSeconds));

            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
    }

    // Appends a string to a file, with file-locking to prevent concurrent write conflicts.
    static void WriteStringToFile(std::string filename, std::string text) {
        HANDLE hFile = CreateFileA(
            filename.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening file: " << GetLastError() << std::endl;
            return;
        }

        OVERLAPPED ol = { 0 };

        if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ol)) {
            std::cerr << "Error locking file: " << GetLastError() << std::endl;
            CloseHandle(hFile);
            return;
        }

        if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
            std::cerr << "Error setting file pointer: " << GetLastError() << std::endl;
            UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
            CloseHandle(hFile);
            return;
        }

        DWORD bytesWritten = 0;
        if (!WriteFile(hFile, text.c_str(), static_cast<DWORD>(text.size()), &bytesWritten, NULL)) {
            std::cerr << "Error writing to file: " << GetLastError() << std::endl;
        }

        if (!UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol)) {
            std::cerr << "Error unlocking file: " << GetLastError() << std::endl;
        }

        CloseHandle(hFile);
    }

    // Writes all histograms to a single file (merging with existing data).
    // Each histogram is written in blocks of 3 lines: name, values (tab-separated), counts (tab-separated)
    static void WriteAllHistogramsToFile(const std::string& filename, const std::map<std::string, std::map<double, long long>>& histograms) {
        // Step 1: Lock the file
        HANDLE hFile = CreateFileA(
            filename.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening file: " << GetLastError() << std::endl;
            return;
        }

        OVERLAPPED ol = { 0 };
        if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ol)) {
            std::cerr << "Error locking file: " << GetLastError() << std::endl;
            CloseHandle(hFile);
            return;
        }

        // Step 2: Read existing content into memory
        LARGE_INTEGER fileSize;
        std::map<std::string, std::map<double, long long>> fileHistograms;

        if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart > 0) {
            std::string fileContent(fileSize.QuadPart, '\0');
            DWORD bytesRead = 0;
            SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
            if (ReadFile(hFile, &fileContent[0], fileSize.QuadPart, &bytesRead, NULL)) {
                std::istringstream iss(fileContent);
                std::string line;
                while (std::getline(iss, line)) {
                    std::string histName = line;

                    std::getline(iss, line);
                    std::istringstream valStream(line);
                    std::vector<double> values;
                    double v;
                    while (valStream >> v) values.push_back(v);

                    std::getline(iss, line);
                    std::istringstream countStream(line);
                    std::vector<long long> counts;
                    long long c;
                    while (countStream >> c) counts.push_back(c);

                    for (size_t i = 0; i < values.size() && i < counts.size(); ++i) {
                        fileHistograms[histName][values[i]] += counts[i];
                    }
                }
            }
        }

        // Step 3: Merge in-memory histograms
        for (const auto& [name, hist] : histograms) {
            for (const auto& [value, count] : hist) {
                fileHistograms[name][value] += count;
            }
        }

        // Step 4: Write updated content back
        std::ostringstream oss;
        for (const auto& [name, hist] : fileHistograms) {
            oss << name << "\n";

            for (const auto& [val, _] : hist) oss << val << "\t";
            oss << "\n";
            for (const auto& [_, cnt] : hist) oss << cnt << "\t";
            oss << "\n";
        }

        std::string finalContent = oss.str();
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        DWORD bytesWritten = 0;
        WriteFile(hFile, finalContent.c_str(), static_cast<DWORD>(finalContent.size()), &bytesWritten, NULL);
        SetEndOfFile(hFile);

        // Step 5: Unlock and close
        UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
        CloseHandle(hFile);
    }

};
