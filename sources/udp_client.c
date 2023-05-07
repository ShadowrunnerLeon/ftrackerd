#include "../headers/udp_client.h"

int main(int argc, char* argv[]) 
{
    SetParametrs(argc, argv);
    sockfd_and_addrindo_ptr res = Initialize();

    Send(res.sockfd, res.ptr);
    Recv(res.sockfd);
    Shutdown(res.sockfd);
}