#include <gtk/gtk.h>
#include <string.h>

#include "types.h"

StringArray get_profiles();

static void
run_profile(GtkWidget* widget, gpointer profile)
{
    FFParams* params = profile;

    char* result = malloc((26 + strlen((*params).profile) + strlen((*params).url)) * sizeof(char));
    strcpy(result, "firefox -p \"");
    strcat(result, (*params).profile);
    strcat(result, "\" \"");
    strcat(result, (*params).url);
    strcat(result, "\" & disown");

    printf("Command: %s\n", result);
    system(result);
}

static void
activate(GtkApplication* app, gpointer profiles)
{
    GtkWidget* window = gtk_application_window_new(app);

    {
        GtkCssProvider* css_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_string(css_provider,
            ".window { font-family: Futura; background: linear-gradient(-60deg, #313236 0%, #40393e 100%); }");
        gtk_style_context_add_provider_for_display(gtk_widget_get_display(window),
            (GtkStyleProvider*)css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
        gtk_widget_add_css_class(window, "window");
    }

    gtk_window_set_title(GTK_WINDOW(window), "FF Profiles");
    // TODO: the appearence is adjusted only for 3 profiles
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 522);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    gtk_window_set_child(GTK_WINDOW(window), box);

    FFParamsAll* paramsAll = profiles;
    for (int i = 0; i < (*paramsAll).profiles.length; i++) {
        GtkWidget* button;
        button = gtk_button_new();

        FFParams* params
            = malloc(sizeof(FFParams));
        (*params).profile = (*paramsAll).profiles.data[i];
        (*params).url = (*paramsAll).url;

        g_signal_connect(button, "clicked", G_CALLBACK(run_profile), params);
        g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
        gtk_widget_set_size_request(button, 170, 200);
        gtk_widget_set_margin_start(button, 10);
        gtk_widget_set_margin_end(button, 10);

        {
            GtkCssProvider* css_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_string(css_provider, ".button { opacity: 0.756; color: #fff; font-size: 16pt; }");
            gtk_style_context_add_provider_for_display(gtk_widget_get_display(button),
                (GtkStyleProvider*)css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_widget_add_css_class(button, "button");
        }

        gtk_box_append(GTK_BOX(box), button);

        GtkWidget* inner_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_button_set_child(GTK_BUTTON(button), inner_box);

        gtk_widget_set_halign(inner_box, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(inner_box, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_start(inner_box, 15);
        gtk_widget_set_margin_end(inner_box, 15);
        gtk_widget_set_margin_top(inner_box, 20);
        gtk_widget_set_margin_bottom(inner_box, 20);

        // TODO: check MAX_LINE_LENGTH is not exceeded
        char filename[MAX_LINE_LENGTH];
        strcpy(filename, getenv("HOME"));
        strcat(filename, "/.ff_profiles/");
        strcat(filename, (*paramsAll).profiles.data[i]);
        GtkWidget* image = gtk_image_new_from_file(filename);
        gtk_widget_set_overflow(image, true);

        {
            GtkCssProvider* css_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_string(css_provider, ".image { border-radius: 50%; }");
            gtk_style_context_add_provider_for_display(gtk_widget_get_display(image),
                (GtkStyleProvider*)css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_widget_add_css_class(image, "image");
        }

        gtk_image_set_pixel_size(GTK_IMAGE(image), 200);

        gtk_box_append(GTK_BOX(inner_box), image);
        gtk_widget_set_margin_bottom(image, 5);
        gtk_widget_set_size_request(image, 200, 200);
        gtk_box_append(GTK_BOX(inner_box), gtk_label_new((*paramsAll).profiles.data[i]));
    }

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char* argv[])
{
    StringArray profiles = get_profiles();
    FFParamsAll paramsAll;
    paramsAll.profiles = profiles;

    if (argc >= 2) {
        paramsAll.url = argv[1];
    } else {
        paramsAll.url = malloc(23 * sizeof(char));
        strcpy(paramsAll.url, "about:home");
    }

    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.ff_profiles", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), &paramsAll);
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);

    return status;
}
