#include "core.h"

static GtkWidget *window;

//used in main window
GtkWidget *treeview;
GtkWidget *starting_combo;
GtkWidget *destination_combo;
GtkWidget *time_combo;
GList *starting_glist = NULL;
GList *destination_glist = NULL;
GList *time_glist = NULL;
GtkWidget *search_button;
GtkWidget *booked_button;
GtkWidget *greet_label;
GtkWidget *id_label;
GtkWidget *id_entry;
GtkWidget *number_entry;
GtkWidget *buy_button;

//used in history dialog
GtkWidget *history_treeview;

//used in login dialog
GtkWidget *login_name_entry;
GtkWidget *login_pwd_entry;

//used in register dialog
GtkWidget *register_name_entry;
GtkWidget *register_pwd_entry;
GtkWidget *register_confirm_pwd_entry;

GtkWidget *create_main_window()
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *topbar_hbox;
    GtkWidget *bottom_hbox;
    GtkWidget *scrolledwindow;
    GtkCellRenderer *renderer;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 540, 480);

    //init combox
    starting_combo = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(starting_combo), starting_glist);
    destination_combo = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(destination_combo), destination_glist);
    time_combo = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(time_combo), time_glist);



    //init the tree view
    treeview = gtk_tree_view_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_ID,
                                                "ID", renderer,
                                                "text", COLUMN_ID,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_STARTING,
                                                "Starting", renderer,
                                                "text", COLUMN_STARTING,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_DESTINATION,
                                                "Destination", renderer,
                                                "text", COLUMN_DESTINATION,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_TIME,
                                                "Time", renderer,
                                                "text", COLUMN_TIME,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_DURATION,
                                                "Duration", renderer,
                                                "text", COLUMN_DURATION,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_TICKET_TOTAL,
                                                "TicketTotal", renderer,
                                                "text", COLUMN_TICKET_TOTAL,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                COLUMN_TICKET_LEFT,
                                                "TicketLeft", renderer,
                                                "text", COLUMN_TICKET_LEFT,
                                                NULL);


    //scrolledwindow contain the tree view
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);

    //init greet label
    greet_label = gtk_label_new("");
    id_label = gtk_label_new("");

    //init the search button
    search_button = gtk_button_new_with_label("Search");
    booked_button = gtk_button_new_with_label("History");
    buy_button = gtk_button_new_with_label("\tBuy\t ");

    //init buy number entry
    number_entry = gtk_entry_new();
    id_entry = gtk_entry_new();

    //manage the layout
    vbox = gtk_vbox_new (FALSE, 0);
    hbox = gtk_hbox_new (FALSE, 0);
    topbar_hbox = gtk_hbox_new(FALSE, 0);
    bottom_hbox = gtk_hbox_new(FALSE, 0);

    //topbar
    gtk_box_pack_start(GTK_BOX(vbox), topbar_hbox, FALSE, FALSE, 5);
    pack_widget_with_label(topbar_hbox, "Hello, ", greet_label);
    //pack_widget_with_label(topbar_hbox, "\tID: ", id_label);
    gtk_box_pack_start(GTK_BOX(topbar_hbox), booked_button, FALSE, FALSE, 5);
    //search bar
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    pack_widget_with_label(hbox, "Starting:", starting_combo);
    pack_widget_with_label(hbox, "Destination:", destination_combo);
    //    pack_widget_with_label(hbox, "Time:", time_combo);
    gtk_box_pack_start(GTK_BOX(hbox), search_button, FALSE, FALSE, 5);
    //content bar
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
    //bottom bar
    pack_widget_with_label(bottom_hbox, "Ticket ID:", id_entry);
    pack_widget_with_label(bottom_hbox, "Number:", number_entry);
    gtk_box_pack_start(GTK_BOX(bottom_hbox), buy_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), bottom_hbox, FALSE, FALSE, 5);


    gtk_container_add(GTK_CONTAINER(window), vbox);

    //handle the signals
    gtk_signal_connect(GTK_OBJECT(window), "destroy",
                       GTK_SIGNAL_FUNC(quit_app), NULL);

    gtk_signal_connect(GTK_OBJECT(search_button), "clicked",
                       GTK_SIGNAL_FUNC(search), NULL);

    gtk_signal_connect(GTK_OBJECT(buy_button), "clicked",
                       GTK_SIGNAL_FUNC(on_buy), NULL);

    gtk_signal_connect(GTK_OBJECT(booked_button), "clicked",
                       GTK_SIGNAL_FUNC(on_history), NULL);


    gtk_widget_show_all(GTK_WIDGET(vbox));

    return window;
}


GtkWidget *create_history_dialog()
{
    GtkCellRenderer *renderer;
    GtkWidget *scrolledwindow;

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Buy History",
                                                    window,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_STOCK_OK,
                                                    GTK_RESPONSE_ACCEPT,
                                                    NULL);

    history_treeview = gtk_tree_view_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(history_treeview),
                                                HIS_COLUMN_SID,
                                                "SID", renderer,
                                                "text", HIS_COLUMN_SID,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(history_treeview),
                                                HIS_COLUMN_NUMBER,
                                                "Number", renderer,
                                                "text", HIS_COLUMN_NUMBER,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(history_treeview),
                                                HIS_COLUMN_TIME,
                                                "Time", renderer,
                                                "text", HIS_COLUMN_TIME,
                                                NULL);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scrolledwindow), history_treeview);


    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), scrolledwindow);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
    gtk_widget_show_all(GTK_WIDGET(GTK_DIALOG(dialog)->vbox));

    return dialog;
}

GtkWidget *create_login_dialog()
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Member Login",
                                                    window,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_STOCK_OK,
                                                    GTK_RESPONSE_ACCEPT,
                                                    GTK_STOCK_CANCEL,
                                                    GTK_RESPONSE_REJECT,
                                                    "Register",
                                                    GTK_RESPONSE_APPLY,
                                                    NULL);
    login_name_entry = gtk_entry_new();
    login_pwd_entry = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(login_pwd_entry), FALSE);

    add_widget_with_label(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), "Name:", login_name_entry);
    add_widget_with_label(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), "Password:", login_pwd_entry);

    gtk_widget_show_all(GTK_WIDGET(GTK_DIALOG(dialog)->vbox));

    return dialog;
}

GtkWidget *create_register_dialog()
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Member Register",
                                                    window,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_STOCK_OK,
                                                    GTK_RESPONSE_ACCEPT,
                                                    GTK_STOCK_CANCEL,
                                                    GTK_RESPONSE_REJECT,
                                                    NULL);

    register_name_entry = gtk_entry_new();
    register_pwd_entry = gtk_entry_new();
    register_confirm_pwd_entry = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(register_pwd_entry), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(register_confirm_pwd_entry), FALSE);

    add_widget_with_label(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), "Name:", register_name_entry);
    add_widget_with_label(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), "Password:", register_pwd_entry);
    add_widget_with_label(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), "Confirm:", register_confirm_pwd_entry);

    gtk_widget_show_all(GTK_WIDGET(GTK_DIALOG(dialog)->vbox));

    return dialog;
}

void add_widget_with_label(GtkContainer *box, gchar *caption, GtkWidget *widget)
{
    GtkWidget *label = gtk_label_new(caption);
    gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);
    GtkWidget *hbox = gtk_hbox_new(TRUE, 5);

    gtk_container_add(GTK_CONTAINER(hbox), label);
    gtk_container_add(GTK_CONTAINER(hbox), widget);

    gtk_container_add(box, hbox);
}

void pack_widget_with_label(GtkWidget *box, gchar *caption, GtkWidget *widget)
{
    GtkWidget *label = gtk_label_new(caption);

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), widget, FALSE, FALSE, 5);
}
