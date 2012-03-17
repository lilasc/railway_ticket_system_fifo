#include "../protocal.h"
#include <ctype.h>
#include "app_mysql.h"


void main()
{
    int server_fifo_fd, client_fifo_fd;
    struct data_to_pass_st my_data;
    int read_res;
    char client_fifo[256];
    char *tmp_char_ptr;

    mkfifo(SERVER_FIFO_NAME, 0777);

    if (database_start("admin", "admin") == -1) {
        fprintf(stderr, "Database connect error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        server_fifo_fd = open(SERVER_FIFO_NAME, O_RDONLY);
        if (server_fifo_fd == -1) {
            fprintf(stderr, "Server fifo failure\n");
            exit(EXIT_FAILURE);
        }

        do {
            char value[10][50];
            read_res = read(server_fifo_fd, &my_data, sizeof(my_data));
            if (read_res > 0) {
                tmp_char_ptr = my_data.data;

                switch (my_data.msg_type) {
                case Login:
                    if (log_in(get_value(tmp_char_ptr, value[0], 0), get_value(tmp_char_ptr, value[1], 1)) == -1)
                        my_data.value_count = -1;
                    else
                        my_data.value_count = 0;
                    break;
                case Register:
                    if (register_mem(get_value(tmp_char_ptr, value[0], 0), get_value(tmp_char_ptr, value[1], 1)) == 0)
                        my_data.value_count = -1;
                    else
                        my_data.value_count = 0;
                    break;
                case SearchTicket:
                    if ((my_data.value_count = search_schedule(tmp_char_ptr, tmp_char_ptr)) == -1)
                        fprintf(stderr, "search_schedule error");
                    break;
                case SearchBuy:
                    my_data.value_count = buy_ticket(get_value(tmp_char_ptr, value[0], 0), get_value(tmp_char_ptr, value[1], 1),
                                                     get_value(tmp_char_ptr, value[2], 2));
                    break;
                case GetInfo:
                    if ((my_data.value_count = get_info(tmp_char_ptr, tmp_char_ptr)) == -1)
                        fprintf(stderr, "get_info error");
                    break;
                case History:
                    if ((my_data.value_count = search_history(tmp_char_ptr, tmp_char_ptr)) == -1)
                        fprintf(stderr, "search_schedule error");
                    break;
                default:
                    fprintf(stderr, "MsgType error");
                    exit(EXIT_FAILURE);
                }

                sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
            }
            client_fifo_fd = open(client_fifo, O_WRONLY);
            if (client_fifo_fd != -1) {
                write(client_fifo_fd, &my_data, sizeof(my_data));
                close(client_fifo_fd);
            }
        } while (read_res > 0);
        close(server_fifo_fd);
    }

    unlink(SERVER_FIFO_NAME);
    database_end();
    exit(EXIT_SUCCESS);
}
