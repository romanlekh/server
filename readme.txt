Hello!
ConsoleChat2015 was create by Roman Lekh, BaseCamp student, with main target - cover gaps in Networking and Multithreading.

I use Code::Blocks with GNU GCC Compiler by Ubuntu, with active using C++11. Please, set in compiler and linker options next info:	
-std=c++11
-pthread
-lpthread

Requirements: 

1.     Server should handle at least 5 clients 
		// As I understood, need to listen() in one time 5 clients. 
2.     Server should support Unicode (at least english/russian language). 
		// My PC has Ubuntu 14.04 and console supports Unicode. At least, I did find some bugs.  
3.     Server should be configured via config files. Following settings should be present: ip, port, max users count. 
		// Done. Discomfort, that need add address of location config_file.txt in Qt-code.  
4.     Client should be able to send/receive simple Unicode messages to all users in chat. 
		// Done. Details below. 

Optional features: 

1.     Server should have a log system/gather statistic. 
		// Server can give info about sent/received messages after input by user “<< stat”. 
2.     Client should be able to send messages to a specific user (private messages). 
		// Done. Details below . 
3.     Support both TCP/UDP mode. Additional bonus for non-server mode: client uses broadcast/multicast for sending messages to other clients. 
		// Not yet.
4.     Chat users can have different access priviledges (admin/user, admin is able to ban user). 
		// Come to chat with name “Admin”. If you want to ban user Tom – “<< ban yes Tom >> “ (without quotes, but with space (also after >>)  ). If you want turn off ban – enter “<< ban not Tom >> ”. 
5.     Chat client should support rich text features (font styles, colors). 
		// Text from server is red and text from other users is green.  
6.     Configure server using GUI. 
		// Done. Folder ServerSetting with Qt project. Sorry, I am not sure about important files Qt-project, so send to you all.


Task was divided by two parts – server and client (sorry, I has started with this in Tuesday and didn't know about rule “all in one”).
 
Server has two .h files. First “client_info.h” (and also client_info.cpp) describes class Client_info. Members are all connected clients with characteristics (like, user_name, user_socket, pointer to user_thread...) and a lot of inline methods. In this shy project not very important constructors (some, inline functions too), but they also exist. Key method – Messenger(...) with loop for receive/send messages between users. Also in this method we check if user wants to quit (bad idea for this user, chat is so funny) or wants to send individual message:
- for exit need input “<< exit” (without quotes but with space before letter “e”);
- for individual message, for example, to user Bob from Barbara, need input “<< Bob >>” (without quotes, but with space before letter “B” and after letter “b”); in this case Bob will receive  “Barbara >> << Bob >> I miss you, Bob!”.

Second file server.h with server.cpp has global variables, like stl::list with users, number of users and limit of user and functions for Server setting from config.file and setting with default parameters. Yes, it must be like normal class.
Main_server.cpp with entry-point and another commands.

Client-part has class Client. Main methods ReceiveMessage() and SendMessage() work in two threads and start in main_client .cpp.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Troubles/bugs.
I believe that sincere confession will be counted and below list with bugs, which I temporary can't solve:

1. After brutal exit Alt-F4 sometimes undefined behavior. I more times terminate server and all users.

2. After normal "<< exit" can be small ..pause, no effect to another users or server. 

   

