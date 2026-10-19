#include "crash_report.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int pipe_fds[2];
    assert(pipe(pipe_fds) == 0);
    pid_t child = fork();
    assert(child >= 0);
    if (!child) {
        struct rlimit limit = {0,0};
        setrlimit(RLIMIT_CORE, &limit);
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDERR_FILENO);
        close(pipe_fds[1]);
        install_crash_reporter();
        raise(SIGSEGV);
        _exit(1);
    }
    close(pipe_fds[1]);
    char output[70000];
    size_t length = 0;
    ssize_t count;
    while ((count = read(pipe_fds[0], output+length, sizeof(output)-1-length)) > 0)
        length += count;
    output[length] = 0;
    close(pipe_fds[0]);
    int status;
    assert(waitpid(child, &status, 0) == child);
    assert(WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV);
    assert(strstr(output, "Hex native crash: SIGSEGV pc=0x"));
    assert(strstr(output, "Memory mappings for crash address decoding:"));
    puts("PASS crash reporting: child signal/location recorded; original signal termination preserved");
}
