#include "core.h"

extern char client_fifo[256];

void quit_app(GtkWidget *window, gpointer data)
{
    unlink(client_fifo);
    gtk_main_quit();
}

gint confirm_exit()
{
    gint result;
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_YES_NO,
                                               "Are you sure you want to quit?");

    result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy (dialog);

    return (result == GTK_RESPONSE_YES);
}

gint show_msg(const char* msg)
{
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               msg);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy (dialog);
}
