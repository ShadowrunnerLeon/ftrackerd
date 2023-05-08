#include "../headers/ftrackerd.h"

int main(int argc, char *argv[]) 
{
    SetParametrs(argc, argv);
    SetSigHandler();

    if (createDaemon(0) == -1) 
    {
        perror("daemon");
        exit(-1);
    }

    Initialize();
    Service();
}