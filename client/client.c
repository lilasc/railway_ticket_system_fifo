#include "core.h"
#include "../protocal.h"
#include <ctype.h>

//used in main window
extern GtkWidget *greet_label;
extern GtkWidget *starting_combo;
extern GtkWidget *destination_combo;
extern GList *starting_glist;
extern GList *destination_glist;
extern GtkWidget *treeview;
extern GtkWidget *id_entry;
extern GtkWidget *number_entry;

//used in history dialog
extern GtkWidget *history_treeview;

//used in login dialog
extern GtkWidget *login_name_entry;
extern GtkWidget *login_pwd_entry;

//used in register dialog
extern GtkWidget *register_name_entry;
extern GtkWidget *register_pwd_entry;
extern GtkWidget *register_confirm_pwd_entry;

int server_fifo_fd, client_fifo_fd;
struct data_to_pass_st my_data;
char client_fifo[256];
char startings[50][50];
char destinations[50][50];

void setup_fifo()
{
    server_fifo_fd = open(SERVER_FIFO_NAME, O_WRONLY);
    if (server_fifo_fd == -1) {
        fprintf(stderr, "Sorry, no server\n");
        exit(EXIT_FAILURE);
    }
    my_data.client_pid = getpid();
    sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
    if (mkfifo(client_fifo, 0777) == -1) {
        fprintf(stderr, "Sorry, can’t make %s\n", client_fifo);
        exit(EXIT_FAILURE);
    }

    my_data.data[0] = '\0';
}

int strequal(const char* s1, const char* s2)
{
    return strcmp(s1, s2) == 0;
}

void get_info()
{
    int i;
    char starting[] = "select distinct beginning from schedule";
    char destination[] = "select distinct destination from schedule";
    char value[256];

    strcpy(my_data.data, starting);
    my_data.msg_type = GetInfo;
    write(server_fifo_fd, &my_data, sizeof(my_data));

    client_fifo_fd = open(client_fifo, O_RDONLY);
    read(client_fifo_fd, &my_data, sizeof(my_data));
    close(client_fifo_fd);

    for (i = 0; i < my_data.value_count; i++) {
        strcpy(startings[i], get_value(my_data.data, value, i));
        starting_glist = g_list_append(starting_glist, startings[i]);
    }
    gtk_combo_set_popdown_strings (GTK_COMBO (starting_combo), starting_glist);

    strcpy(my_data.data, destination);
    my_data.msg_type = GetInfo;
    write(server_fifo_fd, &my_data, sizeof(my_data));

    client_fifo_fd = open(client_fifo, O_RDONLY);
    read(client_fifo_fd, &my_data, sizeof(my_data));
    close(client_fifo_fd);
    for (i = 0; i < my_data.value_count; i++) {
        strcpy(destinations[i], get_value(my_data.data, value, i));
        destination_glist = g_list_append(destination_glist, destinations[i]);
    }

    gtk_combo_set_popdown_strings (GTK_COMBO(destination_combo), destination_glist);
}

void on_history()
{
    GtkWidget *history_dialog;

    GtkTreeStore *tree_store;
    GtkTreeIter parent_iter;
    int i = 0;
    char *data;
    char value[3][50];


    history_dialog = create_history_dialog();

    my_data.msg_type = History;
    strcpy(my_data.data, gtk_label_get_text(GTK_LABEL(greet_label)));
    write(server_fifo_fd, &my_data, sizeof(my_data));

    client_fifo_fd = open(client_fifo, O_RDONLY);
    read(client_fifo_fd, &my_data, sizeof(my_data));
    close(client_fifo_fd);

    tree_store = gtk_tree_store_new (HIS_N_COLUMNS,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    data = my_data.data;
    while (i < my_data.value_count) {
        gtk_tree_store_append (tree_store, &parent_iter, NULL);
        gtk_tree_store_set (tree_store, &parent_iter,
                            HIS_COLUMN_SID, get_value(data, value[0], i * HIS_N_COLUMNS + 0),
                            HIS_COLUMN_NUMBER, get_value(data, value[1], i * HIS_N_COLUMNS + 1),
                            HIS_COLUMN_TIME, get_value(data, value[2], i * HIS_N_COLUMNS + 2),
                            -1
                            );
        i++;
    }
    gtk_tree_view_set_model (GTK_TREE_VIEW (history_treeview), GTK_TREE_MODEL(tree_store));

    gtk_dialog_run(GTK_DIALOG(history_dialog));
    gtk_widget_destroy(history_dialog);
}

void on_buy()
{
    const char *sid;
    const char *mname;
    const char *number;

    sid = gtk_entry_get_text(GTK_ENTRY(id_entry));
    mname = gtk_label_get_text(GTK_LABEL(greet_label));
    number = gtk_entry_get_text(GTK_ENTRY(number_entry));

    if (atoi(number) <= 0) {
        show_msg("Number should larger than 0");
        return;
    }

    sprintf(my_data.data, "%s;%s;%s;", sid, mname, number);
    my_data.msg_type = SearchBuy;
    write(server_fifo_fd, &my_data, sizeof(my_data));

    client_fifo_fd = open(client_fifo, O_RDONLY);
    read(client_fifo_fd, &my_data, sizeof(my_data));
    close(client_fifo_fd);

    if (my_data.value_count != -1)
        show_msg("Done.");
    else
        show_msg("Something error(the id of schedule or the number of ticket), check it.");
    /*
     //the get_tree_model can't get the slected row's info so use a entry instead
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    gchar *value[7];


    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
    //gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(treeview), &model, &iter))
    {
        gtk_tree_model_get (model, &iter,
                            COLUMN_ID, value[0],
                            COLUMN_STARTING, value[1],
                            COLUMN_DESTINATION, value[2],
                            COLUMN_TIME, value[3],
                            COLUMN_DURATION, value[4],
                            COLUMN_TICKET_TOTAL, value[5],
                            COLUMN_TICKET_LEFT, value[6],
                            -1
                            );
        show_msg(value[1]);
    }
    */
}

void search()
{
    char *starting, *destination;
    GtkTreeStore *tree_store;
    GtkTreeIter parent_iter, child_iter;
    int i = 0;
    char *data;
    char value[7][50];

    starting = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO (starting_combo)->entry));
    destination = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO (destination_combo)->entry));

    my_data.msg_type = SearchTicket;
    sprintf(my_data.data, "select * from schedule where beginning='%s' and destination='%s'", starting, destination);
    write(server_fifo_fd, &my_data, sizeof(my_data));

    client_fifo_fd = open(client_fifo, O_RDONLY);
    read(client_fifo_fd, &my_data, sizeof(my_data));
    close(client_fifo_fd);

    tree_store = gtk_tree_store_new (N_COLUMNS,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    data = my_data.data;
    while (i < my_data.value_count) {
        gtk_tree_store_append (tree_store, &parent_iter, NULL);
        gtk_tree_store_set (tree_store, &parent_iter,
                            COLUMN_ID, get_value(data, value[0], i * 7 + 0),
                            COLUMN_STARTING, get_value(data, value[1], i * 7 + 1),
                            COLUMN_DESTINATION, get_value(data, value[2], i * 7 + 2),
                            COLUMN_TIME, get_value(data, value[3], i * 7 + 3),
                            COLUMN_DURATION, get_value(data, value[4], i * 7 + 4),
                            COLUMN_TICKET_TOTAL, get_value(data, value[5], i * 7 + 5),
                            COLUMN_TICKET_LEFT, get_value(data, value[6], i * 7 + 6),
                            -1
                            );
        i++;
    }
    gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL(tree_store));
}

int main(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *login_dialog;
    GtkWidget *register_dialog;
    const gchar *name_const;
    const gchar *pwd_const;
    const gchar *confirm_pwd_const;
    gint result;

    gtk_init(&argc, &argv);
    window = create_main_window();
    login_dialog = create_login_dialog();
    setup_fifo();

    while (1) {
        int can_break = 0;

        result = gtk_dialog_run(GTK_DIALOG(login_dialog));

        if (result == GTK_RESPONSE_REJECT) {
            if (confirm_exit()) {
                unlink(client_fifo);
                return 0;
            }
            else
                continue;
        }
        else if (result == GTK_RESPONSE_APPLY) {
            gtk_widget_destroy(login_dialog);
            register_dialog = create_register_dialog();

            while (1) {
                result = gtk_dialog_run(GTK_DIALOG(register_dialog));
                if (result == GTK_RESPONSE_REJECT) {
                    if (confirm_exit()) {
                        unlink(client_fifo);
                        return 0;
                    }
                    else
                        continue;
                }
                else {
                    name_const = gtk_entry_get_text(GTK_ENTRY(register_name_entry));
                    pwd_const = gtk_entry_get_text(GTK_ENTRY(register_pwd_entry));
                    confirm_pwd_const = gtk_entry_get_text(GTK_ENTRY(register_confirm_pwd_entry));

                    if (strequal(pwd_const, confirm_pwd_const)) {
                        my_data.msg_type = Register;
                        sprintf(my_data.data, "%s;%s;", name_const, pwd_const);
                        write(server_fifo_fd, &my_data, sizeof(my_data));

                        client_fifo_fd = open(client_fifo, O_RDONLY);
                        read(client_fifo_fd, &my_data, sizeof(my_data));
                        close(client_fifo_fd);
                        if (my_data.value_count == -1)
                            show_msg("User name exist");
                        else {
                            gtk_label_set_text(GTK_LABEL(greet_label), name_const);
                            gtk_widget_destroy(register_dialog);
                            can_break = 1;
                            break;
                        }
                    }
                    else
                        show_msg("Two password not equal");
                }
            }
        }
        else {
            name_const = gtk_entry_get_text(GTK_ENTRY(login_name_entry));
            pwd_const = gtk_entry_get_text(GTK_ENTRY(login_pwd_entry));

            my_data.msg_type = Login;
            sprintf(my_data.data, "%s;%s;", name_const, pwd_const);
            write(server_fifo_fd, &my_data, sizeof(my_data));

            client_fifo_fd = open(client_fifo, O_RDONLY);
            read(client_fifo_fd, &my_data, sizeof(my_data));
            close(client_fifo_fd);
            if (my_data.value_count == -1)
                show_msg("User name or password error");
            else {
                gtk_label_set_text(GTK_LABEL(greet_label), (gchar *)name_const);
                gtk_widget_destroy(login_dialog);
                can_break = 1;
            }
        }

        if (can_break) break;
    }

    get_info();

    gtk_widget_show(window);
    gtk_main();

    return 0;
}
