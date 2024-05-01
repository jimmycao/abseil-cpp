#pragma once

#include "net.h"

namespace mlf {

std::string get_ip() {
    std::string hostname(128, '\0');
    std::string ip(128, '\0');

    MCHECK(gethostname((char*)hostname.c_str(), hostname.size()) == 0);
    struct hostent *hent = gethostbyname((char*)hostname.c_str());
    //int ip = ntohl(((struct in_addr*)hent->h_addr)->s_addr);
    const in_addr* in_addr = (struct in_addr*)hent->h_addr;
    MCHECK(inet_ntop(AF_INET, in_addr, (char*)ip.c_str(), ip.size()) != NULL);

    for (size_t i = 0; i  < ip.size(); i++) {
        if(ip[i] == '\0') {
            return ip.substr(0, i);
        }
    }
    return ip;
};

std::string get_hostname() {
    std::string hostname(128, '\0');
    MCHECK(gethostname((char*)hostname.c_str(), hostname.size()) == 0);

    for (size_t i = 0; i  < hostname.size(); i++) {
        if(hostname[i] == '\0') {
            return hostname.substr(0, i);
        }
    }
    return hostname;
}

int get_local_port() {
    unsigned short port = 0;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return port;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = 0; 
    int ret = bind(sock, (struct sockaddr *) &addr, sizeof addr); 
    do {
        if (0 != ret) { 
            break;
        }
        struct sockaddr_in sockaddr;
        int len = sizeof(sockaddr);
        ret = getsockname(sock, (struct sockaddr *) &sockaddr, (socklen_t *) &len);
        if (0 != ret) {
            break;
        }
        port = ntohs(sockaddr.sin_port);  
    } while (0);
    close(sock);
    return port;
}

} // namespace mlf
