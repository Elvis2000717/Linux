#include <stdio.h>
#include <signal.h>

volatile int g_val = 1;

void sigcallback(int sig)
{
    g_val = 0;
    printf("sig: %d\n", sig);
}

int main()
{
    signal(2, sigcallback);
    while(g_val)
    {}

    return 0;
}
