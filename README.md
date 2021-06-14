# ftrackerd

## Descripton
ftrackerd is a daemon that monitors file size changes using fstat () and sends reports to the client over UDP.

## Descripton of libraries
- createdaemon.h - implementation of the daemon creation function.
- logLib.h - functions for working with logs and configs.
- HashTable.h - functions for working with a hash table.

## How to use
Before using it, you need to create files in /tmp:
- ftracker.config
- ftracker.log

Next, fill in tracker. config as follows: the first line contains the number of files to track, each subsequent line corresponds to the file name (use an absolute path). An example of the config is available in this repository.

Example of running the program:

    sudo ./ftrackerd 80 5

The first parameter is responsible for the port, the second - for the waiting time of the daemon.
The daemon can be started without parameters, then the default port value is 16680, and the waiting time is 1 second.

To make sure that the daemon is running, enter the following command in the terminal:

    ps -ef | grep ./ftrackerd

Then you can run udp_client on another device. The main condition is that the devices must be connected to a local network. Example of running udp_client:

    ./udp_client track /tmp/file

When running udp_client, the command to address the daemon is specified. There are four of them in total:

    hello       - display greeting
    track       - display file status(changed or not changed)
    track file1 - display file status(changed or not changed) of file1
    help        - display this commands

If you updated the config while the daemon was running, you need to send it a SIGHUP signal using this command in the terminal:

    sudo killall -HUP ./ftrackerd
    
To disable the daemon, enter:

    sudo killall ./ftrackerd
