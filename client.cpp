#include <iostream>
#include <thread>
#include "common/message.h"
#include "lib/SocketClient.h"
#include "utils.h"

using namespace std;

void onMessage(SocketClient *sender, vector<string> messages){
    for(int i=0 ; i<messages.size() ; i++){
        cout << "message[" << i << "] : " << messages[i] << endl;
    }
}

void onMessage_advertise(SocketClient *sender, vector<string> messages){
    for(int i=0 ; i<messages.size() ; i++){
        cout << "advertise[" << i << "] : " << messages[i] << endl;
    }
}

void onDisconnect(SocketClient *socket){
    cout << "you have been disconnected" << endl;
}

int main(int argc , char *argv[])
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

#ifdef USE_UNIX_DOMAIN
    SocketClient client;
    cout<<"Client try connect to UNIX domain socket"<<endl;;
#else
    SocketClient client(SOCKET_IP_ADDR, SOCKET_PORT);
    cout<<"Client try connect to "<<SOCKET_IP_ADDR<<":"<<SOCKET_PORT<<endl;
#endif
    client.setDisconnectListener(onDisconnect);

    client.addListener("message", onMessage);
    client.addListener("advertise", onMessage_advertise);

    if(!client.connect()){
        cout << "could not connect to server" << endl;
        return 0;
    }
    cout << "connected to server." << endl;

    //register cilent mac
    string&& macs=get_current_mac_addrs();
    cout<<"this client's macs: " << macs<<endl;
    if (!client.send("register", {macs})){
        cout<<"fail to register"<<endl;
        return 0;
    }

    std::string line;
    while(1){
        cout << "input: ";
        getline(cin, line);
        if(!client.send("message", {line})){
            cout << "failed to send message" << endl;
            return 0;
        }
    }

    client.disconnect();

    return 0;
}

