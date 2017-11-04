#include "SocketClient.h"
#include <string.h>

SocketClient::SocketClient(){}

SocketClient::SocketClient(std::string address, int port){
    m_address = address;
    m_port = port;

    m_server.sin_addr.s_addr = inet_addr(address.c_str());
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(port);

    m_tag = NULL;
    m_disconnectListener = NULL;
    m_connected = false;
    m_threadStopped = false;
    m_packetSize = 4096;
}

SocketClient::SocketClient(int socket){
    m_socket = socket;
    m_tag = NULL;
    m_disconnectListener = NULL;
    m_connected = true;
    m_threadStopped = false;
    m_packetSize = 4096;
    pthread_create(&m_thread, NULL, &staticReceiveThread, this);
}

int SocketClient::getSocket(){
    return m_socket;
}

bool SocketClient::connect(){
    m_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(m_socket == -1)
    {
        return false;
    }

    if(::connect(m_socket, (struct sockaddr *)&m_server, sizeof(m_server)) < 0)
    {
        return false;
    }

    m_connected = true;
    pthread_create(&m_thread, NULL, &staticReceiveThread, this);

    return true;
}

void SocketClient::disconnect(){
    close(m_socket);
    m_connected = false;
    m_threadStopped = true;
}

bool SocketClient::send(std::string message){
    uint32_t length = htonl(message.size());
    if(::send(m_socket, &length, sizeof(length), 0) < 0){
        return false;
    }
    if(::send(m_socket, message.c_str(), message.size(), 0) < 0){
        return false;
    }
    return true;
}

bool SocketClient::send(std::string key, std::vector<std::string> messages){
    if(send(key)){
        return send(vectorToString(messages));
    }
    return false;
}

int SocketClient::receive(std::string &message){
    uint32_t length;
    int code;

    code = ::recv(m_socket, &length, sizeof(length), 0);
    if(code!=-1 && code!=0){
        length = ntohl(length);
        char server_reply[length];
        message = "";

        for(int i=0 ; i<length/m_packetSize ; i++){
            code = ::recv(m_socket, server_reply, m_packetSize, 0);
            if(code!=-1 && code!=0){
                message += std::string(server_reply, m_packetSize);
            }
            else{
                return code;
            }
        }
        if(length%m_packetSize!=0){
            char server_reply_rest[length%m_packetSize];
            code = ::recv(m_socket, server_reply_rest, length%m_packetSize, 0);
            if(code!=-1 && code!=0){
                message += std::string(server_reply_rest, length%m_packetSize);
            }
        }
    }
    return code;
}

int SocketClient::receive_buf(void* buf, uint32_t length){
    int code;
    int received = 0;

    while(received < length){
        code = ::recv(m_socket, buf, length, 0);
        if(code < 0){ //TODO
            printf("%s: fail to recv buf, ret=%d\n", __FUNCTION__, code);
            return code;
        }
        if(code == 0){
            printf("%s: fail to recv buf, ret=%d\n", __FUNCTION__, code);
            return code;
        }
        received += code;
    }
    if (received != length){
        printf("%s: request receive=%d, real=%d\n", __FUNCTION__, length, received);
        return 0; //let call disconnect socket
    }
    return received; //return the real received count
}

void SocketClient::addListener(std::string key, void (*messageListener) (SocketClient*, std::vector<std::string>)){
    m_messageListenerMap[key] = messageListener;
}

void SocketClient::setDisconnectListener(void (*disconnectListener) (SocketClient*)){
    m_disconnectListener = disconnectListener;
}

void SocketClient::setTag(void *tag){
    m_tag = tag;
}

void* SocketClient::getTag(){
    return m_tag;
}

/*
void SocketClient::receiveThread(){
    std::string key, message;
    int code1, code2;
    while (!m_threadStopped) {
        code1 = receive(key);
        code2 = receive(message);
        if(code1==0 || code2==0){
            disconnect();
            if(m_disconnectListener!=NULL){
                (*m_disconnectListener)(this);
            }
        }
        else if(code1!=-1 && code2!=-1){
            if(m_messageListenerMap[key]!=NULL){
                (*m_messageListenerMap[key])(this, stringToVector(message));
            }
        }
    }
}
*/

void SocketClient::receiveThread(){
    std::string key, message;
    int code1, code2, code3;
    uint32_t msgId = 0;
    while (!m_threadStopped) {
        //receive msg head
        code1 = receive_buf(&msgId, sizeof(msgId));
        if (msgId == MSG_FRAME_INFO){
            struct MsgFrameInfo msg;
            memset(&msg, 0, sizeof(msg));
            code2 = receive_buf(&msg, sizeof(msg));
            printf("%s: receive msgId=%d, wxh=%dx%d, buflen=%d\n", __FUNCTION__, msgId, msg.width, msg.height, msg.bufLen);

            char* frame = (char*)malloc(msg.bufLen);
            code3 = receive_buf(frame, msg.bufLen);
            free(frame);
        }
        if(code1==0 || code2==0 || code3==0){
            printf("%s: disconnect socket\n", __FUNCTION__);
            disconnect();
            if(m_disconnectListener!=NULL){
                (*m_disconnectListener)(this);
            }
        }
        else if(code1!=-1 && code2!=-1 && code3 != -1){
            if(m_messageListenerMap[key]!=NULL){
               // (*m_messageListenerMap[key])(this, stringToVector(message));
            }
        }
    }
}
