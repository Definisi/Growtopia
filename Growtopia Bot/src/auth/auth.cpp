#include <atlsecurity.h> 
#include <utils/md5.hpp>


#include <auth/auth.hpp>
#include <auth/auth_data.hpp>

void auth::init()
{
	data::hwid = get_hwid();
	data::ip = "";//https://api.ipify.org/
}

void auth::kill_debugger()
{
    const char* processesToKill[] = {
        "HTTPDebuggerUI.exe",
        "HTTPDebuggerSvc.exe",
        "cheatengine*",
        "httpdebugger*",
        "processhacker*",
        "fiddler*",
        "wireshark*",
        "rawshark*",
        "charles*",
        "ida*"
    };

    const char* servicesToStop[] = {
        "HTTPDebuggerPro",
        "KProcessHacker3",
        "KProcessHacker2",
        "KProcessHacker1",
        "wireshark",
        "npf"
    };

    for (const char* process : processesToKill) {
        char command[200];
        snprintf(command, sizeof(command), "taskkill /f /im %s >nul 2>&1", process);
        system(command);
    }

    for (const char* service : servicesToStop) {
        char command[200];
        snprintf(command, sizeof(command), "sc stop %s >nul 2>&1", service);
        system(command);
    }

}

std::string auth::get_hwid()
{
	ATL::CAccessToken access_token;
	ATL::CSid current_user_sid;
	if (access_token.GetProcessToken(TOKEN_READ | TOKEN_QUERY) &&
		access_token.GetUser(&current_user_sid))
		return utils::md5(std::string(CT2A(current_user_sid.Sid())));
	return "none";
}
