#include <iostream>
#include <vector>

#include "Tools.h"
#include "lib/SocketClient.h"
#include "lib/SocketServer.h"

using namespace std;

std::vector<SocketClient*> clientsVector;

void forward(string key, vector<string> messages, SocketClient *exception){
	std::string *_uid = (std::string*) exception->getTag();
	for(auto x : clientsVector){
		std::string *uid = (std::string*) x->getTag();
		if((*uid)!=(*_uid)){
			x->send(key, messages);
		}
	}
}

void onMessage(SocketClient *socket, vector<string> messages){
	forward("message", messages, socket);
}

void onDisconnect(SocketClient *socket){
	cout << "client disconnected !" << endl;
	forward("message", {"Client disconnected"}, socket);
	std::string *_uid = (std::string*) socket->getTag();
	for(int i=0 ; i<clientsVector.size() ; i++){
		std::string *uid = (std::string*) clientsVector[i]->getTag();
		if((*uid)==(*_uid)){
			clientsVector.erase(clientsVector.begin() + i);
		}
	}
	delete socket;
}

void freeMemory(){
	for(auto x : clientsVector){
		delete (std::string*) x->getTag();
		delete x;
	}
}


int main(int argc , char *argv[]){
	srand(time(NULL));

#ifdef USE_UNIX_DOMAIN
    //use unix domain socket
	SocketServer server;
    cout<<"server use unix domain socket " << UNIX_DOMAIN_SOCKET_NAME<<endl;
#else
    //use inter domain socket
	SocketServer server(SOCKET_PORT);
    cout<<"server use inter domain socket "<< SOCKET_IP_ADDR << " : " << SOCKET_PORT << endl;
#endif

	if(server.start()){
		while (1) {
			int sock = server.accept();
			if(sock!=-1){
				cout << "client connected !" << endl;
				SocketClient *client = new SocketClient(sock);
				client->addListener("message", onMessage);
				client->setDisconnectListener(onDisconnect);
				client->setTag(new std::string(getUid()));
				clientsVector.push_back(client);
			}
		}
	}
	else{
		cout << "fail to start server" << endl;
	}

	freeMemory();
	return 0;
}