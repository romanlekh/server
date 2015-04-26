// SERVER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <iterator>
#include <thread>
#include <mutex>
#include <chrono>
#include "server.h"
#include "client_info.h"
using namespace std;

int main()
{
    ifstream config_file("config.txt");
        if( config_file.is_open() )
            ServerSetting(config_file);
        else
            DefaultServerSetting();
    config_file.close();

    // Descriptors for sockets
    int soc_lis;
    int soc_new;
    int res_soc;

    // Set socket
    soc_lis = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
    if( soc_lis < 0 )
    {
        cerr << "Error with soc_lis";
        exit(1);
    }

    // Binding socket with address
    res_soc = bind(soc_lis, (sockaddr*)&g_local, sizeof(g_local));
    if( res_soc < 0 )
    {
        cerr << "Error with bind";
        exit(1);
    }
    else
    {
        cout << "SERVER waiting you, my friend...\n";
    }

    // Set listening
    res_soc = listen(soc_lis, g_users_limit);
    if( res_soc )
    {
        cerr << "Error with listen";
        exit(1);
    }

    // Thread for exit, use global bool variable loop_work
    thread exit_thread(ExitFunction);
    for( ;g_loop_work; this_thread::sleep_for(std::chrono::seconds(2)) )
    {
        cout.flush();
        g_user_count_mutex.lock();
        size_t temp_user_count = g_user_count;
        g_user_count_mutex.unlock();

        // If was found connection, create new thread for this user
        if( temp_user_count < g_users_limit )
            soc_new = accept(soc_lis, NULL, NULL);
        else
            continue;

        if(!g_loop_work)
            break;

        if( soc_new > 0 )
        {
            // Get user_name from client
            char user_name[16] = {'\0'};
            res_soc = recv(soc_new, user_name, sizeof(user_name), 0);

            // Set info about client to Client_info
            Client_info curinfo;
            curinfo.SetName(user_name);
            curinfo.SetSocket(soc_new);

            if( curinfo.GetName() == "Admin" )
                curinfo.SetAdmin(true);


            g_user_count_mutex.lock();
            ++g_user_count;
            size_t temp_user_count = g_user_count;
            g_user_count_mutex.unlock();

            cout << "\nNew client #" << temp_user_count;
            cout << ", name " << user_name << endl;

            // Send conformation to client
            char buf_hello[30] = "Hello ";
            strcat(buf_hello, user_name);
            send(soc_new, buf_hello, sizeof(buf_hello), 0);

            // All next user_name print in chat will be with delimiter symbol
            curinfo.SetName( curinfo.GetName() + " >> " );

            g_user_list_mutex.lock();

            // Add this client to user_set;
            // after that temporary variable curinfo
            // will be deleted, but he have cope in
            // global list user_list
            g_user_list.push_back(curinfo);

            // Create individual thread with function Messanger
            auto temp_iter = g_user_list.end();
            --temp_iter;
            g_user_list.back().SetConnectionStatus(true);
            thread *user_thread = new thread( &Client_info::Messenger, temp_iter);
            g_user_list.back().SetThread(user_thread);

            auto temp_list = g_user_list;
            g_user_list_mutex.unlock();

            // To avoid data-race between r.97 on this page and
            // from r.35 on client_info.cpp
            // I decide to check every element from user_list
            // by false-value in field "connect_status"
            for( auto iter = temp_list.begin(); iter != temp_list.end(); ++iter )
            {
                if( iter->GetConnectionStatus()== false )
                {
                    g_user_list_mutex.lock();
                    delete iter->GetThread();
                    close(iter->GetSocket());
                    g_user_list.erase(iter);
                    g_user_list_mutex.unlock();
                    break;
                }
            }
        }
        cout.flush();
    }

    close(soc_lis);
    exit_thread.join();
    return 0;
}
