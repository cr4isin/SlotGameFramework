
#include "General.h"

void PrintVec(vector<int> input)
{
	for (int i : input)
	{
		cout << i << " ";
	}
	cout << "\n\n";
}

vector<int> ChangeBase(int input, int base, int length)
{
	int quot = abs(input);
	int rem = 0;
	if (length <= 0)
	{
		if (quot <= 0)
		{
			length = 1;
		}
		else
		{
			length = 1 + log(quot) / log(base);
		}
	}
	vector<int> baseVector(length);
	for (int i = 0; i < length; i++)
	{
		rem = quot % base;
		quot /= base;
		baseVector[i] = rem;
	}
	return baseVector;
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

string FormatInteger(int value, int width, char fill) {
	ostringstream oss;
	oss << setfill(fill) << setw(width) << value;
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