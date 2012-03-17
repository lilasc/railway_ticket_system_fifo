#include <gtk/gtk.h>

enum {
    COLUMN_ID,
    COLUMN_STARTING,
    COLUMN_DESTINATION,
    COLUMN_TIME,
    COLUMN_DURATION,
    COLUMN_TICKET_TOTAL,
    COLUMN_TICKET_LEFT,
    N_COLUMNS
};

enum {
    HIS_COLUMN_SID,
    HIS_COLUMN_NUMBER,
    HIS_COLUMN_TIME,
    HIS_N_COLUMNS
};

GtkWidget *create_main_window();

GtkWidget *create_login_dialog();

GtkWidget *create_register_dialog();

GtkWidget *create_history_dialog();

void add_widget_with_label(GtkContainer *box, gchar *caption, GtkWidget *widget);

void pack_widget_with_label(GtkWidget *box, gchar *caption, GtkWidget *widget);

void show_gtk_widgets(GtkWidget *widget);

void quit_app(GtkWidget *window, gpointer data);

void search();

void on_buy();

void on_history();

gint show_msg(const char* msg);

gint confirm_exit();
