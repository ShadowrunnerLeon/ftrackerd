/**
 * implementing a function to create a daemon
 **/
#ifndef CREATEDAEMON_H
#define CREATEDAEMON_H

#define NO_CHDIR            01  //не перенаправлять в "/"
#define NO_CLOSE_FILES      02  //не закрывать все открытые файлы
#define NO_REOPEN_STD_FDS   04  //не перенаправлять stdin, stdout, stderr в "/dev/null"
#define NO_UMASK0          010  //не выполнять umask(0)
#define MAX_CLOSE         8192  //если не определена sysconf(_SC_OPEN_MAX)

int createDaemon(int flags) 
{
    int fd, maxfd;

    switch (fork())
    {
        case -1: return -1;
        case 0:  break;
        default: _exit(0);
    }

    if (setsid() == -1) return -1;

    switch (fork())
    {
        case -1: return -1;
        case 0:  break;
        default: _exit(0);
    }

    if (!(flags && NO_UMASK0)) umask(0);
    if (!(flags && NO_CHDIR)) chdir("/");

    if (!(flags && NO_CLOSE_FILES)) 
    {
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1) maxfd = MAX_CLOSE;
        for (fd = 0; fd < maxfd; ++fd) close(fd);
    }

    if (!(flags && NO_REOPEN_STD_FDS)) 
    {
        close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO) return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) return -1;
    }

    return 0;

}

#endif