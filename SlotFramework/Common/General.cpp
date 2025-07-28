
#include "General.h"

void PrintPauseMessage(string message, bool clearAfter)
{
	string input;
	cout << message << flush;
	getline(cin, input);
	if (clearAfter) cout << ANSI::PreviousLine(1) << ANSI::ClearLine() << flush;
}

void SpawnProcesses(string programName, int numTimes, int delayInSeconds)
{
	// Convert the executable name to a wide string and create the full command line
	wstring fullCommandLine = L"\"" + wstring(programName.begin(), programName.end()) + L"\" 1";

	// Create new processes
	for (int i = 0; i < numTimes; ++i) {
		// CreateProcess parameters
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInfo;

		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory(&processInfo, sizeof(processInfo));

		// Create new process
		if (!CreateProcess(
			NULL,                      // Module name (use NULL to indicate the executable name)
			const_cast<LPWSTR>(fullCommandLine.c_str()),  // Command line
			NULL,                       // Process handle not inheritable
			NULL,                       // Thread handle not inheritable
			FALSE,                      // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,         // Creation flags
			NULL,                       // Use parent's environment block
			NULL,                       // Use parent's starting directory
			&startupInfo,               // Pointer to STARTUPINFO structure
			&processInfo                // Pointer to PROCESS_INFORMATION structure
		)) {
			cerr << "Error creating process. Exiting." << endl;
			return;
		}

		this_thread::sleep_for(chrono::seconds(delayInSeconds));

		// Close process and thread handles
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
}

string FormatDouble(double value, int precision)
{
	ostringstream oss;
	if (precision < 0)
	{
		oss << fixed << setprecision(numeric_limits<double>::max_digits10) << value;
	}
	else
	{
		oss << fixed << setprecision(precision) << value;
	}
	return oss.str();
}

string FormatInt(int value, int width, char fill) {
	ostringstream oss;
	oss << setfill(fill) << setw(width) << value;
	return oss.str();
}

string FormatString(string value, int width, char fill, bool alignLeft)
{
	ostringstream oss;
	oss << setfill(fill) << setw(width) << (alignLeft ? left : right) << value;
	return oss.str();
}

std::string FormatTime(long long milliseconds) {
	long long seconds = milliseconds / 1000;
	long long minutes = seconds / 60;
	long long hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;
	ostringstream oss;
	oss << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << ":" << setw(2) << seconds;
	return oss.str();
}

// This function aggregates the in-memory histogram with any existing data in the file.
// It uses Windows API file locking to ensure exclusive access.
void WriteHistogramToFile(string filename, map<double, long long> hist) {
	// Open the file for read and write. 
	// OPEN_ALWAYS creates the file if it doesn't exist.
	HANDLE hFile = CreateFileA(
		filename.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // Allow other processes to open the file
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening file: " << GetLastError() << std::endl;
		return;
	}

	// Prepare an OVERLAPPED structure required for locking.
	OVERLAPPED ol = { 0 };

	// Lock the entire file exclusively.
	// Without LOCKFILE_FAIL_IMMEDIATELY, this call will block until the lock is available.
	if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ol)) {
		std::cerr << "Error locking file: " << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}

	// Get the current file size.
	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(hFile, &fileSize)) {
		std::cerr << "Error getting file size: " << GetLastError() << std::endl;
		UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
		CloseHandle(hFile);
		return;
	}

	std::string fileContent;
	if (fileSize.QuadPart > 0) {
		// Read the entire file content.
		fileContent.resize(fileSize.QuadPart);
		DWORD bytesRead = 0;
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		if (!ReadFile(hFile, &fileContent[0], fileSize.QuadPart, &bytesRead, NULL)) {
			std::cerr << "Error reading file: " << GetLastError() << std::endl;
			UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
			CloseHandle(hFile);
			return;
		}
	}
	// If the file was just created, its size will be zero and fileContent remains empty.

	// Parse the file content (if any) into a temporary histogram.
	// The file is assumed to have one entry per line in the format: <key><tab><value>
	std::map<double, long long> fileHistogram;
	std::istringstream iss(fileContent);
	std::string line;
	while (std::getline(iss, line)) {
		std::istringstream lineStream(line);
		double key;
		long long value;
		if (lineStream >> key >> value)
			fileHistogram[key] += value;
	}

	// Merge the file's histogram into the in-memory histogram.
	// This adds the counts from the file to those in 'hist'.
	for (const auto& pair : fileHistogram) {
		hist[pair.first] += pair.second;
	}

	// Prepare the new aggregated content.
	std::ostringstream oss;
	for (const auto& pair : hist) {
		oss << pair.first << "\t" << pair.second << "\n";
	}
	std::string newContent = oss.str();

	// Write the new content to the file.
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	DWORD bytesWritten = 0;
	if (!WriteFile(hFile, newContent.c_str(), static_cast<DWORD>(newContent.size()), &bytesWritten, NULL)) {
		std::cerr << "Error writing file: " << GetLastError() << std::endl;
		UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
		CloseHandle(hFile);
		return;
	}

	// Truncate the file in case the new content is shorter than the previous content.
	if (!SetEndOfFile(hFile)) {
		std::cerr << "Error truncating file: " << GetLastError() << std::endl;
		UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
		CloseHandle(hFile);
		return;
	}

	// Unlock the file so that other processes waiting on the lock can proceed.
	if (!UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol))
		std::cerr << "Error unlocking file: " << GetLastError() << std::endl;

	CloseHandle(hFile);
}

// Appends the provided text to the file specified by filename.
// If the file doesn't exist, it will be created.
// Uses Windows API file locks to ensure exclusive access.
void WriteStringToFile(string filename, string text) {
	// Open (or create) the file with write access.
	// FILE_SHARE_READ | FILE_SHARE_WRITE allows other processes to open the file,
	// but our lock below will ensure exclusive access for writing.
	HANDLE hFile = CreateFileA(
		filename.c_str(),           // File name.
		GENERIC_WRITE,              // Write access.
		FILE_SHARE_READ | FILE_SHARE_WRITE, // Sharing mode.
		NULL,                       // Default security.
		OPEN_ALWAYS,                // Create the file if it doesn't exist.
		FILE_ATTRIBUTE_NORMAL,      // Normal file.
		NULL                        // No template.
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening file: " << GetLastError() << std::endl;
		return;
	}

	// Prepare an OVERLAPPED structure required for LockFileEx.
	OVERLAPPED ol = { 0 };

	// Lock the entire file exclusively.
	// Without the LOCKFILE_FAIL_IMMEDIATELY flag, this call will block until the lock is available.
	if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ol)) {
		std::cerr << "Error locking file: " << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}

	// Move the file pointer to the end so that we append new data.
	if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
		std::cerr << "Error setting file pointer: " << GetLastError() << std::endl;
		UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
		CloseHandle(hFile);
		return;
	}

	// Write the provided text to the file.
	DWORD bytesWritten = 0;
	if (!WriteFile(hFile, text.c_str(), static_cast<DWORD>(text.size()), &bytesWritten, NULL)) {
		std::cerr << "Error writing to file: " << GetLastError() << std::endl;
		UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol);
		CloseHandle(hFile);
		return;
	}

	// Unlock the file so other processes can access it.
	if (!UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &ol)) {
		std::cerr << "Error unlocking file: " << GetLastError() << std::endl;
	}

	// Close the file handle.
	CloseHandle(hFile);
}
