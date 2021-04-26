#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigcallback(int sig)
{
    printf("sig: %d\n", sig);
}

int main()
{
    signal(2, sigcallback);
    signal(40, sigcallback);
    sigset_t set;
    sigfillset(&set);
    sigset_t oldset;
    sigprocmask(SIG_SETMASK, &oldset, NULL);
    
    while(1)
    {
        sleep(1);
    }

    return 0;
}
