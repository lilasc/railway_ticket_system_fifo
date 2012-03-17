#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096
#define SERVER_FIFO_NAME "/tmp/serv_fifo"
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo"

enum MsgType {
    Login,
    Register,
    SearchTicket,
    SearchBuy,
    GetInfo,
    History
};

struct data_to_pass_st {
    pid_t client_pid;
    int value_count;
    enum MsgType msg_type;
    char data[BUFFER_SIZE - 1];
};

char* get_value(const char* data, char* const out, int index)
{
    int i = 0;
    const char* data_ptr = data;
    char* out_ptr = out;

    while (i < index) {
        while (*data_ptr++ != ';')
            ;
        i++;
    }
    while ((*out_ptr++ = *data_ptr++) != ';')
        ;
    *--out_ptr = '\0';

    return out;
}
