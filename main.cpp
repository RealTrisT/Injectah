#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <mbstring.h>

#include "Otherwise.h"
#include "Inject.h"

#define IM_THREAD_HIJACK 0x1
#define IM_CT_LL 0x2

HINSTANCE hInst; HWND window;
std::vector <std::wstring> ToInject;
std::vector<DWORD> ProcIDs; 
int choice = -1;
byte InjectionMode = 0x0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//--------------------------------------------------------WINDPROC
{
	switch (message)
	{
	case WM_CREATE:
	{
		int x = 0;
		int y = 0;
		for (size_t i = 0; i < ToInject.size(); i++) //generate the buttons
		{
			if (i % 2 == 0) { //if it's a button that goes on the right (if it's an even index)
				if (i == ToInject.size()-1) x = 130;//if it's an odd number of buttons and the last one, place it at the center
				else x = 10 + 240;
				y = (int)(10 + 40 * (i/2));
			}
			else {//if it's a button that goes on the left (if it's an odd number)
				x = 10;
				y = (int)(trunc(i / 2) * 40 + 10);

			}
			HWND hwmdButon = CreateWindow( //inb4 yes, you create buttons with CreateWindow
				L"BUTTON", 
				(ToInject[i].substr(0, ToInject[i].find(L".exe")) + L" (" + std::to_wstring(ProcIDs[i]) + L")").c_str(), 
				BS_FLAT | WS_VISIBLE | WS_CHILD,
				x, 
				y, 
				230, 30, //button size
				hWnd, 
				(HMENU)i, //button index that will be received as the wParam in WM_COMMAND
				hInst, //Program's Hinstance
				NULL);
		}
		break;
	}
	case WM_COMMAND: //When a button is clicked. wParam contains the index.
	{
		choice = (int)wParam;
		DestroyWindow(hWnd); //Set the global variable to the index value and destroy the window so we can continue synchronously
	}
	case WM_KILLFOCUS:
		if(GetActiveWindow() == NULL) //When clicking a button WM_KILLFOCUS is sent, so we need to check that it's still our program's window that's on top.
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY://pretty self explanatory :D
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {//------ENTRY POINT
	hInst = hInstance;//put hInstance into the Global Variable
	
	//manage arguments---------------------------------------------------------->
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);
	int argc = 0; 
	LPWSTR *argvW;
	argvW = CommandLineToArgvW((LPCWSTR)GetCommandLineW(), &argc);
	//--------------------------------------------------------------------------<


	//check arguments----------------------------------------------------------->
	bool argumentsOK = true;
	if (argc < 2){argumentsOK = !debugW(L"No File Chosen", L"Injectah");}
	if (!PathFileExists(argvW[1])){ argumentsOK = !debugW(L"Path Provided Isn't a Valid Path", L"Injectah");}//if path provided isn't a valid path
	std::wstring inipath(argvW[0]);
	inipath = inipath.substr(0, inipath.find_last_of(L"\\/")+1) + L"injectah.ini";
	if (!PathFileExists(inipath.c_str())){ argumentsOK = !debugW(L"No injectah.ini In Injector Folder", L"Injectah");}//if .ini exists
	if (argc > 2) { 
		if (wcscmp(argvW[2], L"/h") == 0) InjectionMode = IM_THREAD_HIJACK; //third argument /m do manual mapping
		else if (wcscmp(argvW[2], L"/l") == 0)InjectionMode = IM_CT_LL;  //third argument /l do createthread loadlibrary
		else InjectionMode = IM_CT_LL; }//unknown mode default to createthread loadlibrary
	else { InjectionMode = IM_CT_LL; }//no third argument default to createthread loadlibrary
	if (!argumentsOK) { LocalFree(argvW); return false; }
	//--------------------------------------------------------------------------<


	/*
	//make ANSI version of path string-------------> 
	//(Initially thinking on using someone else's manual mapping function (since I can't make one yet), which took an ANSI string, leaving it in in case anyone wants to use their own and needs ANSI)
	size_t origsize = wcslen(argvW[1]) + 1;	size_t convertedChars = 0;
	const size_t newsize = origsize * 2; char *pathA = new char[newsize];
	wcstombs_s(&convertedChars, pathA, newsize, argvW[1], _TRUNCATE);
	//--------------------------------------------------------------------------<
	*/

	//Get Ini File Processes---------------------------------------------------->
	std::vector<std::wstring> iniEntries;

	std::wifstream ini(inipath.c_str());
	if (!ini.is_open()){LocalFree(argvW); return debugW(L"Failed To Open injectah.ini", L"Injectah");}
	wchar_t buffer[256]; std::string buffa;
	while (ini.good()) {
		ini.getline(buffer, 256);
		iniEntries.push_back(buffer);
	}
	ini.close();//--------------------------------------------------------------<



	//Listing Processes And Their IDs------------------------------------------->
	PROCESSENTRY32 ProcessInfo = { sizeof(PROCESSENTRY32) };
	HANDLE SnapShot; SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == INVALID_HANDLE_VALUE) {LocalFree(argvW); return !debugW(L"CreateToolhelp32Snapshot Failed"); }

	if (Process32First(SnapShot, &ProcessInfo)) {
		do {
			for (size_t i = 0; i < iniEntries.size(); i++)
			{
				if (!wcscmp(ProcessInfo.szExeFile, iniEntries[i].c_str())) {
					ToInject.push_back(ProcessInfo.szExeFile);
					ProcIDs.push_back(ProcessInfo.th32ProcessID);
				}
			}
		} while (Process32Next(SnapShot, &ProcessInfo));
	}
	CloseHandle(SnapShot);//---------------------------------------------------<



	bool single = false;
	if (ToInject.size() == 0)return debugW(L"No Open Processes To Inject To", L"Injectah");//No Open Listed Processes
	else if (ToInject.size() == 1) { 
		single = true;
	}
	else {
		single = false;
		//Setting Up The Window------------------------------------------------->
		WNDCLASSEX wcex;//default window shenanigans

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;//No Icon
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); //Default Cursor
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;//No Name
		wcex.lpszClassName = L"ClassNaem";
		wcex.hIconSm = NULL;//No Icon

		if (!RegisterClassEx(&wcex)){return !debugW(L"Call to RegisterClassEx failed!", std::to_wstring(GetLastError()).c_str());}

		int height = (int)(50 + 40 * ((ToInject.size() % 2 == 0) //if it's NOT a even number of buttons
					? (ToInject.size() / 2) //make space for X pairs of buttons
					: (1 + (int)trunc(ToInject.size() / 2))));//make room for pairs of 2 and one more for the odd
		COORD pos;
		pos.X = GetSystemMetrics(SM_CXSCREEN)/2 - 510/2;		//Centered Window Horizontally
		pos.Y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2; //Centered Window Vertically

		window = CreateWindow(
			L"ClassNaem",
			NULL, //no window name
			WS_BORDER, //no borders
			pos.X, pos.Y, //coords
			510, //width
			height, 
			NULL,
			NULL,
			hInstance,
			NULL
		);
		if (!window){debugW(L"Call to CreateWindow failed!"); LocalFree(argvW);	return true;}
		ShowWindow(window, nCmdShow);
		UpdateWindow(window);
		//----------------------------------------------------------------------<
	}


	//Message Loop-------------------------------------------------------------->
	MSG msg;
	if (!single) {
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//--------------------------------------------------------------------------<


	
	//Option Chosen------------------------------------------------------------->
	if (choice != -1 || single) {
		if (InjectionMode == IM_CT_LL)
			if (LoadLibraryInject(argvW[1], ToInject[single ? 0 : choice].c_str())) { //Inject With CreateThread and LoadLibrary
				//debugW(L"Injected Successfully!");
			}
			else { debugW(L"CreateThread LoadLibrary Injection Failed", L"Injectah"); }
		else if (InjectionMode == IM_THREAD_HIJACK) {
			if (ThreadHijackInject(argvW[1], ToInject[single ? 0 : choice].c_str())) { //Inject with Thread Hijacking
				//debugW(L"Injected Successfully!");
			}
			else { debugW(L"Thread Hijacking Injection Failed", L"Injectah"); }
		}
	}
	//cleanup------------------------------------------------------------------->
	LocalFree(argvW);
	return single?true:(int)msg.wParam;
	//------------------------------------------------------------------------------------------------Exit Point>
}