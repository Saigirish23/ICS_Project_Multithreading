#include "B24CH1046_B24CM1021_B24MT1013_B24ME1066_firewater_.h"

// Application startup
void on_start(GtkApplication *app, gpointer user_data) {
    // Initialize game state
    memset(&game_state, 0, sizeof(GameState));
    game_state.current_level = 1;
    game_state.fireboy.lives = MAX_LIVES;
    game_state.watergirl.lives = MAX_LIVES;
    strcpy(game_state.game_message, "");
    pthread_mutex_init(&game_state.lock, NULL);
    
    // Create window
    GtkWidget *window = gtk_application_window_new(app);
    game_state.window = window;
    gtk_window_set_title(GTK_WINDOW(window), "Fireboy & Watergirl");
    gtk_window_set_default_size(GTK_WINDOW(window), MAP_WIDTH * TILE_SIZE, 
                              MAP_HEIGHT * TILE_SIZE + PROGRESS_BAR_HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    
    // Load images
    GError *error = NULL;
    game_state.fireboy.sprite = gdk_pixbuf_new_from_file("FireBoy.png", &error);
    game_state.watergirl.sprite = gdk_pixbuf_new_from_file("WaterGirl.png", &error);
    game_state.lava_tile = gdk_pixbuf_new_from_file_at_scale("lava.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.water_tile = gdk_pixbuf_new_from_file_at_scale("water.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.ground_tile = gdk_pixbuf_new_from_file_at_scale("ground.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.fire_exit_tile = gdk_pixbuf_new_from_file_at_scale("fireexit.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.water_exit_tile = gdk_pixbuf_new_from_file_at_scale("waterexit.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.fire_gem_image = gdk_pixbuf_new_from_file_at_scale("firegem.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.water_gem_image = gdk_pixbuf_new_from_file_at_scale("watergem.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.fence_tile = gdk_pixbuf_new_from_file_at_scale("fence.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.hazard_image = gdk_pixbuf_new_from_file_at_scale("hazard.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    game_state.pressure_plate_image = gdk_pixbuf_new_from_file_at_scale("pressureplate.png", TILE_SIZE, TILE_SIZE, FALSE, &error);
    
    if (error) {
        g_printerr("Error loading images: %s\n", error->message);
        g_error_free(error);
        return;
    }
    
    // Generate first level
    generate_map();
    spawn_gems();
    
    // Create main container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Create drawing area
    game_state.drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(game_state.drawing_area, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE);
    gtk_widget_add_events(game_state.drawing_area, GDK_KEY_PRESS_MASK);
    gtk_widget_set_can_focus(game_state.drawing_area, TRUE);
    
    // Create progress bar container
    GtkWidget *progress_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(progress_box, MAP_WIDTH * TILE_SIZE, PROGRESS_BAR_HEIGHT);
    
    // Create progress bars
    game_state.fire_progress = gtk_progress_bar_new();
    gtk_widget_set_size_request(game_state.fire_progress, (MAP_WIDTH * TILE_SIZE) / 2, PROGRESS_BAR_HEIGHT);
    game_state.water_progress = gtk_progress_bar_new();
    gtk_widget_set_size_request(game_state.water_progress, (MAP_WIDTH * TILE_SIZE) / 2, PROGRESS_BAR_HEIGHT);
    
    // Add widgets to containers
    gtk_box_pack_start(GTK_BOX(progress_box), game_state.fire_progress, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(progress_box), game_state.water_progress, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), game_state.drawing_area, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), progress_box, FALSE, FALSE, 0);
    
    // Connect signals
    g_signal_connect(G_OBJECT(game_state.drawing_area), "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), NULL);
    
    // Show window
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(game_state.drawing_area);
    
    // Initialize progress bars
    update_progress_bars();
    
    // Start game threads
    pthread_t t_fire, t_water, t_input;
    pthread_create(&t_input, NULL, input_thread, NULL);
    pthread_create(&t_fire, NULL, fireboy_thread, NULL);
    pthread_create(&t_water, NULL, watergirl_thread, NULL);
    
    // Start game loop
    g_timeout_add(100, game_loop, NULL);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.FireWaterGame", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_start), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}