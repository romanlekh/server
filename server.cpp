#include <list>
#include <string>
#include <iterator>
#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "client_info.h"

using std::string;
using std::iterator;
using std::cerr;
using std::cin;
using std::endl;
using std::cout;
using std::mutex;


// Global variables have suffix "g_"
std::list<Client_info> g_user_list;
size_t g_users_limit = 5;
size_t g_user_count = 0;
std::mutex g_user_list_mutex;
std::mutex g_user_count_mutex;
sockaddr_in g_local;

// For loop in main() function
bool g_loop_work = true;
std::mutex g_loop_work_mutex;

// ip, port, max users count
void ServerSetting(std::ifstream& config_file)
{
    string temp_string;
    while( config_file >> temp_string )
        {}

    // Parser for IP
    size_t delim_1 = temp_string.find(':');
        if( delim_1 == string::npos )
        {
            DefaultServerSetting();
            return;
        }
    string ip_str = temp_string.substr(0, delim_1);

    // Parser for port
    size_t delim_2 = temp_string.find(':', delim_1 + 1);
        if( delim_2 == string::npos )
        {
            DefaultServerSetting();
            return;
        }
    string port_str = temp_string.substr( delim_1 + 1, delim_2 - delim_1 - 1);

    // Parser for max count of users
    string max_users = temp_string.substr( delim_2 + 1);

    // Set configuration
    g_users_limit = std::stoi(max_users);
    // Set socket type
    g_local.sin_family = AF_INET;
    // Set port
    g_local.sin_port = htons( std::stoi(port_str) );
    // Set addresses, what can connect server
    g_local.sin_addr.s_addr = inet_addr( ip_str.c_str() );
}

void DefaultServerSetting()
{
    // Set info about me
    g_local.sin_family = AF_INET;
    // Set port
    g_local.sin_port = htons(7500);
    // Set addresses, what can connect server
    g_local.sin_addr.s_addr = htonl( INADDR_ANY );
    g_users_limit = 5;
}

void ExitFunction()
{
    for(;;std::this_thread::sleep_for(std::chrono::seconds(10)) )
    {
        cout << endl << "Enter \"<< exit\" for exit" << endl
        << "(server will turn off only after successful accept)." << endl;
        string exit_string;
        getline(cin, exit_string);
        if( exit_string == "<< exit" )
        {
            g_loop_work_mutex.lock();
            g_loop_work = false;
            g_loop_work_mutex.unlock();
            return;
        }
    }
}
