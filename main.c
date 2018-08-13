#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/timerfd.h>

void error(const char* msg);
void errorx(const char *msg);

void ignore_lines(void);

void wexecute(time_t sec, long nsec, const char *cmd);

int main(int argc, const char *argv[])
{
    int argi = 1;
    const char *arg;
    const char *cmd = NULL;
    /* 1 second is 1000000000 nsecs */
    time_t sec = 0;
    long nsec = 250000000;
    char *endptr;

    for (; argi < argc; ++argi)
    {
        arg = argv[argi];

        if (!strcmp(arg, "-n") || !strcmp(arg, "--nanoseconds"))
        {
            if (argi > argc - 2)
                errorx("Invalid --nanoseconds argument.");

            nsec = strtol(argv[++argi], &endptr, 10);
            if (*endptr)
                errorx("Invalid --nanoseconds value.");
        }
        else if (!strcmp(arg, "-s") || !strcmp(arg, "--seconds"))
        {
            if (argi > argc - 2)
                errorx("Invalid --seconds argument.");

            sec = strtol(argv[++argi], &endptr, 10);
            if (*endptr)
                errorx("Invalid --seconds value.");
        }
        else if (!strcmp(arg, "-c") || !strcmp(arg, "--command"))
        {
            if (argi > argc - 2)
                errorx("Invalid --command argument.");

            cmd = argv[++argi];
        }
        else
        {
            errorx("Invalid argument.");
        }
    }

    if (!cmd) errorx("Missing command.");

    /* Make sure stdin is non-blocking, so we can read everything. */
    /*int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); */


    wexecute(sec, nsec, cmd);
    
    return EXIT_SUCCESS;
}

void wexecute(time_t sec, long nsec, const char *cmd)
{
    int tfd = -1;
    struct itimerspec t;
    uint64_t tcount = 0;
    struct pollfd pfds[2];
    int recent = 0;

    if (-1 == (tfd = timerfd_create(CLOCK_MONOTONIC, 0)))
        error("timerfd_create()");

    t.it_interval.tv_sec = 0;
    t.it_interval.tv_nsec = 0;
    t.it_value.tv_sec = sec;
    t.it_value.tv_nsec = nsec;

    if (-1 == (timerfd_settime(tfd, 0, &t, NULL)))
        error("timerfd_settime()");

    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    pfds[0].revents = 0;

    pfds[1].fd = tfd;
    pfds[1].events = POLLIN;
    pfds[1].revents = 0;

    while (1)
    {
        if (-1 == poll(pfds, 2, -1))
            error("poll()");

        /* If there's no more standard input, stop. */
        if (pfds[0].revents & POLLHUP) break;

        /* If there is input on standard in... */
        if (pfds[0].revents & POLLIN) 
        {
            ignore_lines();
            recent = 1;

            if (-1 == (timerfd_settime(tfd, 0, &t, NULL)))
                error("timerfd_settime()");
        }

        /* If the timer expired. */
        if (pfds[1].revents & POLLIN)
        {
            read(tfd, &tcount, sizeof(tcount));

            if (recent)
            {
                system(cmd);
                recent = 0;
            }
        }
    }

    close(tfd);
}

void error(const char* msg)
{
    err(EXIT_FAILURE, "%s", msg);
}

void errorx(const char *msg)
{
    errx(EXIT_FAILURE, "%s", msg);
}

void ignore_lines(void)
{
    char *line = NULL;
    size_t size = 0;

    while (1)
    {
        getline(&line, &size, stdin);
        free(line);

        return;
    }
}

