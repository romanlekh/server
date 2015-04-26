#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "client_info.h"

using std::list;
using std::string;
using std::iterator;
using std::cerr;
using std::cin;
using std::endl;
using std::cout;
using std::mutex;

mutex messenger_mutex;

Client_info::Client_info() {}

void Client_info::Messenger(list<Client_info>::iterator user)
{
    for(;;)
    {
        // Receive messages form user
        char buf[1024] = {'\0'};
        size_t buf_size = sizeof(buf);
        int res_soc = recv( user->user_socket, buf, buf_size, 0);
        if( res_soc <= 0 )
        {
            g_user_count_mutex.lock();
            --g_user_count;
            g_user_count_mutex.unlock();

            g_user_list_mutex.lock();
            user->connect_status = false;
            string temp_user_name = user->user_name;
            g_user_list_mutex.unlock();

            cerr << "\nError with recv in Messanger";
            cout << "\nUser " << temp_user_name << "out\n";
            // user will be delete after checking in r.107 on main_server.cpp
            return;
        }
        else
        {
            string buf_string = user->user_name;
            cout.flush();

            // Check if user wants to quit
            if( strstr(buf, "<< exit") != NULL )
            {
                ExitMethod(user, buf_string, buf, buf_size);
                return;
            }

            // Check if user wants to see his statistic
            if( strstr(buf, "<< stat") != NULL )
            {
                StatisticView(user, buf_string, buf, buf_size);
                continue;
            }

            // Check if user wants to see his statistic
            if( strstr(buf, "<< ban") != NULL && user->admin_user == true)
            {
                cout << "\nFIRST" << endl;
                AdminBan(user, buf_string, buf, buf_size);
                continue;
            }

            // Check if message is individual
            if( strstr(buf, "<<") != NULL && strlen(buf) != 0 )
            {
                IndividualMessenger(user, buf_string, buf, buf_size);
                continue;
            }

            // Send this message to all users until position of this user in user_list
            if( strlen(buf) != 0 )
                TotalSend(user, buf_string, buf, buf_size);
        }
    }
}


void Client_info::IndividualMessenger(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size)
{
    // If user is banned - he does not play
    if(user->ban == true)
        return;

    g_user_list_mutex.lock();
    auto temp_list = g_user_list;
    g_user_list_mutex.unlock();

    auto iter = find_if(temp_list.begin(), temp_list.end(),
                        [&buf]( const Client_info& cl )
                        {
                            return strstr(buf+2, cl.user_name.c_str());
                        });

    if( iter != temp_list.end() )
    {
        buf_string += buf;
        cout << buf_string;
        int res_soc = send(iter->user_socket, buf_string.c_str(), buf_size, 0);
        if( res_soc <= 0 )
        {
            cerr << "Error with send in IndividualMessenger";
        }
        ++iter->recv_messages;
    }
    ++iter->send_messages;
    cout.flush();
}

void Client_info::StatisticView(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size)
{
    buf_string += "Statistic: sent messages " + std::to_string(user->send_messages) +
    ", received messages " + std::to_string(user->recv_messages);
    int res_soc = send(user->user_socket, buf_string.c_str(), buf_size, 0);
    if( res_soc <= 0 )
    {
        cerr << "Error with send in Stat";
    }
}

void Client_info::ExitMethod(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size)
{
    g_user_count_mutex.lock();
    --g_user_count;
    g_user_count_mutex.unlock();

    buf_string += "leave us.";
    cout << "\nUser " << user->user_name << "out\n" << endl;

    // This and below I make copy of user_list
    // because for my opinion it is faster
    // than lock original user_list every time
    g_user_list_mutex.lock();
    auto temp_list = g_user_list;
    user->connect_status = false;
    g_user_list_mutex.unlock();

    temp_list.erase(user);
    // Send to other users info about deserter
    int res_soc;
    for( auto iter = temp_list.begin(); iter != temp_list.end(); ++iter )
        res_soc = send(iter->user_socket, buf_string.c_str(), buf_size, 0);
    if( res_soc <= 0 )
    {
        cerr << "Error with send in Exit";
    }

    cout.flush();
}

void Client_info::TotalSend(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size)
{
    // If user is banned - he does not play
    if(user->ban == true)
        return;

    buf_string += buf;
    cout << buf_string << endl;
    // Descriptor for socket
    int res_soc;
    g_user_list_mutex.lock();
    for( auto iter = g_user_list.begin(); iter != user; ++iter )
    {
        res_soc = send(iter->user_socket, buf_string.c_str(),
                    buf_size, 0);
        if( res_soc <= 0 )
        {
            cerr << "Error with send until";
        }
    }
    auto temp_user = user;
    ++temp_user;
    // Send this message to other users after position of this user in user_list
    for( auto iter = temp_user; iter != g_user_list.end(); ++iter )
    {
        res_soc = send(iter->user_socket, buf_string.c_str(),
                    buf_size, 0);
        if( res_soc <= 0 )
        {
            cerr << "Error with send after";
        }
    }
    ++user->send_messages;
    g_user_list_mutex.unlock();
    cout.flush();
}

void Client_info::AdminBan(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size)
{
    g_user_list_mutex.lock();
    auto iter = find_if(g_user_list.begin(), g_user_list.end(),
                        [&buf]( const Client_info& cl )
                        {
                            return strstr(buf+11, cl.user_name.c_str());
                        });

    if( iter != g_user_list.end() )
    {
        if(strncmp(buf+7, "yes", 3) == 0)
        {
            // this guy is banned
            iter->ban = true;
            cout << iter->user_name << " banned!";
        }
        else
        {
            // this guy is free
            iter->ban = false;
        }
    }
    g_user_list_mutex.unlock();
    cout.flush();
}






