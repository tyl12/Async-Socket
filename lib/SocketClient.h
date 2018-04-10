#ifndef DEF_SOCKETCLIENT
#define DEF_SOCKETCLIENT

#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>

#include "Base64.h"
#include "message.h"

class SocketClient {
    private:
        void *m_tag;

        struct sockaddr_in m_server;
        std::string m_address;
        int m_port;
        int m_socket;
        bool m_connected;
        bool m_threadStopped;

        int m_packetSize;

        pthread_t m_thread;
        std::map<std::string, void (*)(SocketClient*, std::vector<std::string>)> m_messageListenerMap;
        void (*m_disconnectListener) (SocketClient*);

        void receiveThread();
        static void* staticReceiveThread(void* p){
            SocketClient *client = (SocketClient*) p;
            client->receiveThread();
        }
        int receive(std::string &message);
        bool send(std::string message);

        int receive_buf(void* buf, uint32_t length);

    public:
        SocketClient();
        SocketClient(std::string address, int port);
        SocketClient(int socket);

        int getSocket();
        void* getTag();

        bool connect();
        void disconnect();
        bool send(std::string key, std::vector<std::string> messages);
        void addListener(std::string key, void (*messageListener) (SocketClient*, std::vector<std::string>));
        void setDisconnectListener(void (*disconnectListener) (SocketClient*));
        void setTag(void *tag);
};

#endif
