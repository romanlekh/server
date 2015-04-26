#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


#include <netinet/in.h>
#include "client_info.h"


extern size_t g_user_count;
extern std::list<Client_info> g_user_list;
extern std::mutex g_user_list_mutex;
extern std::mutex g_user_count_mutex;
extern size_t g_users_limit;
extern sockaddr_in g_local;

extern bool g_loop_work;
extern std::mutex g_loop_work_mutex;

// Server configured via config file.
// Following settings present: ip, port, max users count
// Default and only one try name for file - "config_file.txt" (without  quotation marks)
void ServerSetting(std::ifstream& config_file);

// Default setting for server: IP INADDR_ANY, port 7500, max users count 5
void DefaultServerSetting();

// Exit Function from Server
void ExitFunction();


#endif // SERVER_H_INCLUDED







