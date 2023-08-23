#include "comm.h"

#include <stdarg.h>
#include <util.h>
#include <unistd.h>

static int master_fd;

bool comm_init(char* serial_port)
{
    int slave_fd;

    if (openpty(&master_fd, &slave_fd, serial_port, NULL, NULL) == -1) {
        perror("openpty");
        return false;
    }

    // configure terminal settings
    struct termios tio;
    tcgetattr(master_fd, &tio);
    cfmakeraw(&tio);
    tcsetattr(master_fd, TCSANOW, &tio);

    return true;
}

void comm_readline(char* line, size_t max_size)
{
    size_t i = 0;
    while (i < max_size) {
        size_t num_bytes = read(master_fd, &line[i], 1);   // TODO - poll
        if (num_bytes > 0) {
            if (line[i] == 13 || line[i] == 10) {
                line[i] = '\0';
                return;
            }
            ++i;
        } else {
            usleep(10000);
        }
    }
}

void comm_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    int sz = vsnprintf(buffer, sizeof buffer, fmt, args);
    write(master_fd, buffer, sz);

    va_end(args);
}