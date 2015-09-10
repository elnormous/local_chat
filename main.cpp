#include <iostream>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

GtkWidget* window = NULL;
GtkWidget* textView = NULL;
GtkWidget* scrollWindow = NULL;
GtkWidget* nickBox = NULL;
GtkWidget* messageBox = NULL;

GtkTextTag* boldTag = NULL;
GtkTextTag* italicTag = NULL;
GtkTextTag* colorTag = NULL;

const char DEFAULT_TYPE = 0;
const char NICK_TYPE = 1;
const char ANONYMOUS_TYPE = 2;

static void destroy(GtkWidget* widget, gpointer data)
{
	gtk_main_quit();
}

static void appendText(GtkTextView* textView, const char* text, char type)
{
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(textView);
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(buffer, &iter);

	switch (type)
    {
        case DEFAULT_TYPE:
        {
            gtk_text_buffer_insert(buffer, &iter, text, -1);
            break;
        }


        case NICK_TYPE:
        {
            gtk_text_buffer_insert_with_tags(buffer, &iter, text, -1, boldTag, colorTag, NULL);
            break;
        }

        case ANONYMOUS_TYPE:
        {
            gtk_text_buffer_insert_with_tags(buffer, &iter, text, -1, italicTag, colorTag, NULL);
            break;
        }
    }
}

static void send(GtkWidget* widget, gpointer data)
{
    const char* nick = gtk_entry_get_text(GTK_ENTRY(nickBox));
    const char* message = gtk_entry_get_text(GTK_ENTRY(messageBox));

    char nickType = NICK_TYPE;

    if (strlen(nick) == 0)
    {
        nick = "anonīms";
        nickType = ANONYMOUS_TYPE;
    }

    // message must not be empty
    if (strlen(message) == 0)
    {
        return;
    }

    appendText(GTK_TEXT_VIEW(textView), nick, nickType);
    appendText(GTK_TEXT_VIEW(textView), ": ", nickType);

    appendText(GTK_TEXT_VIEW(textView), message, DEFAULT_TYPE);
    appendText(GTK_TEXT_VIEW(textView), "\n", DEFAULT_TYPE);

    // log to file
    time_t t = time(0);
    struct tm* now = localtime(&t);

    FILE* file = fopen("log.txt", "a");
    fprintf(file, "[%d.%02d.%02d %02d:%02d:%02d] %s: %s\n",
            now->tm_year + 1900,
            now->tm_mon + 1,
            now->tm_mday,
            now->tm_hour,
            now->tm_min,
            now->tm_sec,
            nick, message);
    fclose(file);
    // log time

    // reset widgets
    gtk_entry_set_text(GTK_ENTRY(nickBox), "");
    gtk_entry_set_text(GTK_ENTRY(messageBox), "");

    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollWindow));
    gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrollWindow), adj);
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));

    //gtk_widget_grab_focus(nickBox);
    gtk_widget_grab_focus(textView);
}

int main(int argc, char* argv[])
{
	PangoFontDescription* font = pango_font_description_from_string("Sans 20");

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // chat box
	textView = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textView), FALSE);

	gtk_widget_override_font(textView, font);

	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
	boldTag = gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
	italicTag = gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
	colorTag = gtk_text_buffer_create_tag(buffer, "color", "foreground", "#0500AA", NULL);


    scrollWindow = gtk_scrolled_window_new(NULL, gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(textView)));
    //gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_container_add(GTK_CONTAINER(scrollWindow), textView);

	gtk_box_pack_start(GTK_BOX(vbox), scrollWindow, TRUE, TRUE, 0);

    // bottom
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

    // nick
    nickBox = gtk_entry_new();
    gtk_widget_set_size_request(nickBox, 200, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(nickBox), "vārds");
    gtk_box_pack_start(GTK_BOX(hbox), nickBox, FALSE, TRUE, 0);

	gtk_widget_override_font(nickBox, font);

    // message
    messageBox = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(messageBox), "ziņa");
    gtk_box_pack_start(GTK_BOX(hbox), messageBox, TRUE, TRUE, 0);
	g_signal_connect(messageBox, "activate", G_CALLBACK(send), NULL);

	gtk_widget_override_font(messageBox, font);

	GtkWidget* sendButton = gtk_button_new_with_label("Sūtīt");
	gtk_box_pack_start(GTK_BOX(hbox), sendButton, FALSE, TRUE, 0);
	g_signal_connect(sendButton, "clicked", G_CALLBACK(send), NULL);

	gtk_widget_override_font(sendButton, font);

    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

    // go follscreen
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_widget_show_all(window);
	gtk_window_fullscreen(GTK_WINDOW(window));
	//gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	/*GdkGeometry hints;
	hints.max_width = 400;
	hints.max_height = 300;

	gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &hints, GDK_HINT_MAX_SIZE);*/

    pango_font_description_free(font);

	gtk_main();

	return 0;
}
