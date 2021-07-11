#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

class TcpClient {
    private:
        int sock;
        std::string address;
        string response_data = "";
        int port;
        struct sockaddr_in server;

    public:
        TcpClient();
        bool conn(string, int);
        bool send_data(string data);
        string receive(int);
};

TcpClient::TcpClient() {
    sock = -1;
    port = 0;
    address = "";
}

bool TcpClient::conn(string address, int port) {
    if(sock == -1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }

    if(inet_addr(address.c_str()) == INADDR_NONE) {
        struct hostent *he;
        struct in_addr **addr_list;

        if ( (he = gethostbyname( address.c_str() ) ) == nullptr) {
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != nullptr; i++) {
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    } else {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    if( connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0 ) {
        perror("connect failed. Error");
        return false;
    }

    cout<<"Connected\n";
    return true;
}

bool TcpClient::send_data(string data) {
    cout<<"Sending data...";
    cout<<data;
    cout<<"\n";
    
    if( send(sock, data.c_str(), strlen( data.c_str() ), 0) < 0) {
        perror("Send failed : ");
        return false;
    }
    
    cout<<"Data send\n";

    return true;
}

string TcpClient::receive(int size=512) {
    char buffer[size];
    string reply;
    int nsize = 0;

    if ((nsize = recv(sock, buffer, sizeof(buffer), 0)) < 0) {
        puts("recv failed");
        return "";
    }

    reply = string(buffer, nsize);
    response_data = reply;
    
    return reply;
}

//int main(int argc , char *argv[])
int main() {
    TcpClient c;
    string host = "127.0.0.1";
    int port = 9999;

    c.conn(host, port);

    while(true) {
	    //string cmd, key, value;
	    //cout << "Enter command (get, put, exit)" << endl;
	    //cin >> cmd;
	    //cout << cmd << endl;
	    //if (cmd == "exit") {
	    //        c.send_data("-quit\n");
	    //        cout<<c.receive(1024);
	    //        break;
	    //} else {
	    //        cout<<"Enter Key Name: " << endl;
	    //        cin >> key;
	    //        cout << key << endl;
	    //        if (cmd == "put") {
	    //    	    cout<<"Enter Value : " << endl;
	    //    	    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    //    	    std::getline(std::cin, value);
	    //    	    cout << value << endl;
	    //    	    c.send_data("-"+cmd+" <"+key+"> <"+value+">\n");
	    //        } else {
	    //    	    c.send_data("-"+cmd+" <"+key+">\n");
	    //        }
	    //        cout<<c.receive(1024);
	    //}
	    //cout<<"\n----------------------------\n";
	    c.send_data("-put <lllll> <mmmmmm>\n");
	    cout<<c.receive(1024);
	    c.send_data("-get <lllll>\n");
	    cout<<c.receive(1024);
	    c.send_data("-quit\n");
	    cout<<c.receive(1024);
	    break;
    }
    return 0;
}
