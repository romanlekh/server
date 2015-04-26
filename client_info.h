#ifndef CLIENT_INFO_H_INCLUDED
#define CLIENT_INFO_H_INCLUDED

#include <string>
#include <iterator>
#include <mutex>
#include <thread>
#include <list>
#include "client_info.h"
#include "server.h"

extern std::mutex messenger_mutex;

class Client_info
{
private:
    std::string user_name;
    int user_socket;
    std::thread *user_thread = nullptr;
    bool connect_status = false;
    bool can_delete = false;
    size_t send_messages = 0;
    size_t recv_messages = 0;

    // Admin can be only one
    static const int num_admin = 0;
    bool admin_user = false;
    bool ban = false;

    // Methods below are parts of Messenger(...)
    // Functions' name says you what she does
    static void IndividualMessenger(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size);
    static void StatisticView(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size);
    static void ExitMethod(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size);
    static void TotalSend(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size);
    static void AdminBan(std::list<Client_info>::iterator& user,
                             std::string& buf_string, const char *buf, size_t buf_size);

public:
    Client_info();

    inline void SetName(const std::string& _name) { user_name = _name; }
    inline void SetSocket(int descriptor) { user_socket = descriptor; }
    inline void SetThread(std::thread *_user_thread) { user_thread = _user_thread; }
    inline void SetConnectionStatus(bool status) { connect_status = status; }
    inline void SetAdmin(bool admin_mark)
    {
         if( num_admin == 0 )
         {
             admin_user = true;
             ++admin_user;
         }
    }

    inline const std::string& GetName() { return user_name; }
    inline int GetSocket() { return user_socket; }
    inline const std::thread* GetThread() { return user_thread; }
    inline bool GetConnectionStatus() { return connect_status; }
    inline bool CheckAdmin() { return admin_user; }

    // Loop messenger
    // param iterator to new client in yser_list.
    // This method can be separate function or method from server.cpp,
    // but in first case enter to private data (by user_list) will be closed;
    // in second the same and I don't want make friends' classes
    // (at least, at this moment server.h/cpp don't have any class)
    static void Messenger( std::list<Client_info>::iterator user );
};



#endif // CLIENT_INFO_H_INCLUDED
