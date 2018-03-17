#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
int main()
{
    int ep, fd, ret;
    struct epoll_event ev, events[1];
    
    ep = epoll_create(1);
    sigset_t set;
    sigemptyset(&set); 
    sigaddset(&set, 4);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    fd = signalfd(-1, &set, 0);

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    ret = epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ev);

    for(;;)
    {
        ret = epoll_wait(ep, events, 1, -1);
        if (ret > 0)
        {
            if (0 != (events[0].events & EPOLLIN))
            {
                struct signalfd_siginfo info;
                ret = read(events[0].data.fd, &info, sizeof(struct signalfd_siginfo));
                printf("ret %d  --  info : %x\r\n",ret,  info.ssi_signo);

            }
        }
    }
    

    return 0;
    

}
