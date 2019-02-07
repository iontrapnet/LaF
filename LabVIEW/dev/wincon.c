#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

HWND GetConsoleWindow();
__declspec(dllexport) void wincon(long hwnd)
{
int hConHandle;
long lStdHandle;
FILE *fp;

if (hwnd == -1) { FreeConsole(); return; }

// allocate a console for this app
if (!(hwnd == 0 ? AllocConsole() : AttachConsole(hwnd))) return;

EnableMenuItem(GetSystemMenu(GetConsoleWindow(), 0), 0xF060, 2);

// redirect unbuffered STDOUT to the console
lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
fp = _fdopen( hConHandle, "w" );
*stdout = *fp;
setvbuf( stdout, NULL, _IONBF, 0 );

// redirect unbuffered STDIN to the console
lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
fp = _fdopen( hConHandle, "r" );
*stdin = *fp;
setvbuf( stdin, NULL, _IONBF, 0 );

// redirect unbuffered STDERR to the console
lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
fp = _fdopen( hConHandle, "w" );
*stderr = *fp;
setvbuf( stderr, NULL, _IONBF, 0 );

// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
// ios::sync_with_stdio();
}