// Linux crash breadcrumbs. Only async-signal-safe calls run in the handler;
// the signal is re-raised with its default action after recording the location.
#include "crash_report.h"
#ifdef __linux__
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <ucontext.h>

static void write_bytes(const char *bytes, size_t length) {
    while (length) {
        ssize_t count = write(STDERR_FILENO, bytes, length);
        if (count <= 0) break;
        bytes += count;
        length -= count;
    }
}

static void write_hex(uintptr_t value) {
    char digits[2 + sizeof(value) * 2];
    digits[0] = '0'; digits[1] = 'x';
    for (size_t i = 2; i < sizeof(digits); ++i) {
        unsigned shift = (sizeof(digits) - 1 - i) * 4;
        digits[i] = "0123456789abcdef"[(value >> shift) & 15];
    }
    write_bytes(digits, sizeof(digits));
}

#define CRASH_TEXT(s) write_bytes(s, sizeof(s) - 1)
static void report_crash(int number, siginfo_t *info, void *raw_context) {
    CRASH_TEXT("\nHex native crash: ");
    switch (number) {
    case SIGSEGV: CRASH_TEXT("SIGSEGV"); break;
    case SIGBUS: CRASH_TEXT("SIGBUS"); break;
    case SIGABRT: CRASH_TEXT("SIGABRT"); break;
    case SIGFPE: CRASH_TEXT("SIGFPE"); break;
    case SIGILL: CRASH_TEXT("SIGILL"); break;
    }
    CRASH_TEXT(" pc=");
    uintptr_t pc = 0;
#if defined(__x86_64__)
    pc = ((ucontext_t *)raw_context)->uc_mcontext.gregs[REG_RIP];
#elif defined(__aarch64__)
    pc = ((ucontext_t *)raw_context)->uc_mcontext.pc;
#endif
    write_hex(pc);
    CRASH_TEXT(" address=");
    write_hex((uintptr_t)info->si_addr);
    CRASH_TEXT("\nMemory mappings for crash address decoding:\n");
    int fd = open("/proc/self/maps", O_RDONLY);
    if (fd >= 0) {
        char bytes[1024];
        size_t remaining = 65536;
        while (remaining) {
            size_t wanted = remaining < sizeof(bytes) ? remaining : sizeof(bytes);
            ssize_t count = read(fd, bytes, wanted);
            if (count <= 0) break;
            write_bytes(bytes, count);
            remaining -= count;
        }
        close(fd);
    }
    struct sigaction action = {0};
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    sigaction(number, &action, NULL);
    kill(getpid(), number);
}

void install_crash_reporter(void) {
    static char alternate_stack[65536];
    stack_t stack = {0};
    stack.ss_sp = alternate_stack;
    stack.ss_size = sizeof(alternate_stack);
    sigaltstack(&stack, NULL);
    struct sigaction action = {0};
    action.sa_sigaction = report_crash;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    const int numbers[] = {SIGSEGV, SIGBUS, SIGABRT, SIGFPE, SIGILL};
    for (unsigned i = 0; i < sizeof(numbers)/sizeof(numbers[0]); ++i)
        sigaction(numbers[i], &action, NULL);
}
#else
void install_crash_reporter(void) {}
#endif
