#include "pch.h"


//BOOL KillProcess(string _sProcessName)
//{
//	HANDLE         hProcessSnap = NULL;
//	DWORD          Return = FALSE;
//	PROCESSENTRY32 pe32 = { 0 };
//
//	string ProcessName = _sProcessName;
//	transform(ProcessName.begin(), ProcessName.end(), ProcessName.begin(), ::tolower);
//	//ProcessName.MakeLower();
//
//
//	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//
//	if (hProcessSnap == INVALID_HANDLE_VALUE)
//		return (DWORD)INVALID_HANDLE_VALUE;
//
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//
//	if (Process32First(hProcessSnap, &pe32))
//	{
//		DWORD Code = 0;
//		DWORD dwPriorityClass;
//
//		do {
//			HANDLE hProcess;
//			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
//			dwPriorityClass = GetPriorityClass(hProcess);
//
//			string Temp = pe32.szExeFile;
//			transform(ProcessName.begin(), ProcessName.end(), ProcessName.begin(), ::tolower);
//			Temp.MakeLower();
//
//			if (Temp == ProcessName)
//			{
//				if (TerminateProcess(hProcess, 0))
//					GetExitCodeProcess(hProcess, &Code);
//				else
//					return GetLastError();
//			}
//			CloseHandle(hProcess);
//		} while (Process32Next(hProcessSnap, &pe32));
//		Return = TRUE;
//	}
//	else
//	{
//		Return = FALSE;
//	}
//
//	CloseHandle(hProcessSnap);
//
//	return Return;
//}

BOOL ProcessAllKill(wstring szProcessName)
{
	//시스템 프로세스에 대한 전체 스냅샷 찍기
	HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD dwsma = GetLastError();
	HANDLE hHandle;

	DWORD dwExitCode = 0;

	//스냅샷 찍은 프로세스에 대한 실행파일 이름, 프로세스 정보, PPID에 대한 정보를 저장한 구조체
	PROCESSENTRY32 procEntry = { 0 };
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	//스냅샷에 첫번째 프로세스를 검색
	Process32First(hndl, &procEntry);

	while (1)
	{
		//실행파일의 이름을 비교하여 같으면 OpenProcess 실행
		if (!wcscmp(procEntry.szExeFile, szProcessName.c_str()))
		{
			//존재하는 프로세스 객체 열기(모든접근, 0, PID)
			hHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, procEntry.th32ProcessID);

			//특정 프로세스의 종료 상태 검색(handle, 프로세스 상태값) - return nonzero
			if (GetExitCodeProcess(hHandle, &dwExitCode))
			{
				//특정 프로세스나 그것의 스레드 전부를 종료
				if (TerminateProcess(hHandle, dwExitCode))
				{
					return TRUE;
				}
			}
		}

		//시스템 스냅샷에 등록된 다음 프로세스에 대한 정보를 검색(스냅샷, processEntry32 구조체)
		if (!Process32Next(hndl, &procEntry))
		{
			return FALSE;
		}
	}

	return FALSE;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::wstring stringToWString(const std::string& str) {
	wstring temp;
	temp.assign(str.begin(), str.end());

	return temp;
}

void PrintExternalIP()
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://ifconfig.me");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		else
		{
			std::wstring wReadBuffer = stringToWString(readBuffer);
			std::wcout << "External IP: " << wReadBuffer << std::endl;
		}

		curl_easy_cleanup(curl);
	}
}

std::wstring exec(const wchar_t* cmd)
{
	SetConsoleOutputCP(CP_UTF8);

	wchar_t buffer[128];
	std::wstring result = L"";
	FILE* pipe = _wpopen(cmd, L"rt, ccs=UTF-8");
	if (!pipe) throw std::runtime_error("_wpopen() failed!");
	try {
		while (!feof(pipe)) {
			if (fgetws(buffer, 128, pipe) != NULL)
				result += buffer;
		}
	}
	catch (...) {
		_pclose(pipe);
		throw;
	}
	_pclose(pipe);
	return result;
}

std::wstring PrintLocalIP()
{
	std::wstring ipconfigResult = exec(L"ipconfig");

	// IPv4 주소를 찾기 위한 정규표현식
	std::wregex ipv4Regex(L"IPv4 Address[\\. ]+: ([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)");

	// 정규표현식에 매칭되는 IPv4 주소 출력
	std::wsmatch matches;
	std::wsregex_iterator it(ipconfigResult.begin(), ipconfigResult.end(), ipv4Regex), end;
	for (std::wsregex_iterator it(ipconfigResult.begin(), ipconfigResult.end(), ipv4Regex), end; it != end; ++it) {
		//std::wcout << "IPv4 Address: " << it->str(1) << std::endl;
		return it->str(1);
	}
	return std::wstring();
}
