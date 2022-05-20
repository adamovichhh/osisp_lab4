#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define CHILDS_AMOUNT 2
int isFirstChild = 0;
int messageCount = 0;

char* GetTime()
{
    struct timeval time;

    if (gettimeofday(&time, NULL) == -1)
        perror("Can not get current time\n");
    else {
        int msec = time.tv_usec / 1000;
        int seconds = time.tv_sec % 60;
        int minutes = (time.tv_sec / 60) % 60;
        int hours = (time.tv_sec / 60 * 60 + 3) % 24;

        char* timeStr = (char*)calloc(25, sizeof(char));
        sprintf(timeStr, "%02d:%02d:%02d:%03d", hours, minutes, seconds, msec);
        return timeStr;
    }
    return "";
}

int SendUSR1()
{
    printf("%2d POST SIGUSR1 PARENT   PID: %d PPID: %d Time %s\n", messageCount++, getpid(), getppid(), GetTime());
    if (kill(0, SIGUSR1) == -1)
        perror("Can not send signal\n");
}

int SendUSR2()
{
    printf("%2d POST SIGUSR2 CHILD #%d PID: %d PPID: %d Time %s\n", messageCount++, isFirstChild ? 1 : 2, getpid(), getppid(), GetTime());
    if (kill(getppid(), SIGUSR2))
        perror("Can not send signal\n");
}

void ParentHandler(int sig, siginfo_t* siginfo, void* code)
{
    printf("%2d GET  SIGUSR2 PARENT   PID: %d PPID: %d Time %s - %d\n", messageCount++, getpid(), getppid(), GetTime(), siginfo->si_pid);
    usleep(100 * 1000);

    SendUSR1();
}

void ChildHandler(int sig, siginfo_t* siginfo, void* code)
{
    printf("%2d GET  SIGUSR1 CHILD #%d PID: %d PPID: %d Time %s\n", messageCount++, isFirstChild ? 1 : 2, getpid(), getppid(), GetTime());

    SendUSR2();
}

int main() {
    struct sigaction parentAction;
    parentAction.sa_sigaction = ParentHandler;
    parentAction.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR2, &parentAction, NULL) == -1) {
        perror("Error: can not set signal handler");
        exit(-1);
    }

    signal(SIGUSR1, SIG_IGN);

    struct sigaction childAction;
    childAction.sa_sigaction = ChildHandler;
    childAction.sa_flags = SA_SIGINFO;

    for (int i = 0; i < CHILDS_AMOUNT; i++) {
        pid_t child = fork();

        switch (child) {
        case 0:
            isFirstChild = i == 0 ? 1 : 0;

            if (sigaction(SIGUSR1, &childAction, NULL) == -1) {
                perror("Error: can not set signal handler");
                exit(-1);
            }
            printf("CHILD #%d PID: %d PPID: %d Time: %s \n", i + 1, getpid(), getppid(), GetTime());
            while (1) {}
        case -1:
            perror("Error: can not create child process");
            exit(-1);
        }
    }

    printf("PARENT   PID: %d PPID: %d Time: %s \n", getpid(), getppid(), GetTime());

    sleep(1);

    SendUSR1();

    while (1) {}
}