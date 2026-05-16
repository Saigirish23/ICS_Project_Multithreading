#include "B24CH1046_B24CM1021_B24MT1013_B24ME1066_firewater_.h"

// Draw callback for GTK
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    // Draw tiles from original map (preserves all special tiles)
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            GdkPixbuf *tile_image = NULL;
            switch (game_state.original_map[y][x]) {
                case TILE_FENCE: tile_image = game_state.fence_tile; break;
                case TILE_LAVA: tile_image = game_state.lava_tile; break;
                case TILE_WATER: tile_image = game_state.water_tile; break;
                case TILE_GROUND: tile_image = game_state.ground_tile; break;
                case TILE_FIRE_EXIT: tile_image = game_state.fire_exit_tile; break;
                case TILE_WATER_EXIT: tile_image = game_state.water_exit_tile; break;
                case TILE_PRESSURE_PLATE: tile_image = game_state.pressure_plate_image; break;
            }
            if (tile_image) {
                gdk_cairo_set_source_pixbuf(cr, tile_image, x * TILE_SIZE, y * TILE_SIZE);
                cairo_paint(cr);
            }
        }
    }
    
    // Draw hazards on top of the original tiles
    for (int i = 0; i < game_state.hazard_count; i++) {
        if (game_state.hazards[i].active) {
            gdk_cairo_set_source_pixbuf(cr, game_state.hazard_image, 
                                      game_state.hazards[i].x * TILE_SIZE, 
                                      game_state.hazards[i].y * TILE_SIZE);
            cairo_paint(cr);
        }
    }
    
    // Draw fireboy collectibles
    for (int i = 0; i < game_state.fire_collectibles; i++) {
        if (!game_state.fire_gems[i].collected && !game_state.fire_gems[i].hidden) {
            gdk_cairo_set_source_pixbuf(cr, game_state.fire_gem_image, 
                                      game_state.fire_gems[i].x, game_state.fire_gems[i].y);
            cairo_paint(cr);
        }
    }

    // Draw watergirl collectibles
    for (int i = 0; i < game_state.water_collectibles; i++) {
        if (!game_state.water_gems[i].collected && !game_state.water_gems[i].hidden) {
            gdk_cairo_set_source_pixbuf(cr, game_state.water_gem_image, 
                                      game_state.water_gems[i].x, game_state.water_gems[i].y);
            cairo_paint(cr);
        }
    }
    
    // Draw fireboy and watergirl
    gdk_cairo_set_source_pixbuf(cr, game_state.fireboy.sprite, game_state.fireboy.x, game_state.fireboy.y);
    cairo_paint(cr);
    gdk_cairo_set_source_pixbuf(cr, game_state.watergirl.sprite, game_state.watergirl.x, game_state.watergirl.y);
    cairo_paint(cr);
    
    // Draw game message if any
    if (strlen(game_state.game_message) > 0) {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, 0, (MAP_HEIGHT * TILE_SIZE) / 2 - 50, MAP_WIDTH * TILE_SIZE, 100);
        cairo_fill(cr);
        
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 40);
        
        cairo_text_extents_t extents;
        cairo_text_extents(cr, game_state.game_message, &extents);
        double x = (MAP_WIDTH * TILE_SIZE - extents.width) / 2;
        double y = (MAP_HEIGHT * TILE_SIZE) / 2 + extents.height / 2;
        
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, game_state.game_message);
    }
    
    return FALSE;
}

// Key press callback for GTK
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (game_state.game_over) return TRUE;
    
    pthread_mutex_lock(&game_state.lock);
    
    switch (event->keyval) {
        // Fireboy controls (WASD)
        case GDK_KEY_w:
        case GDK_KEY_W:
            game_state.last_key_fire = 'w';
            break;
        case GDK_KEY_a:
        case GDK_KEY_A:
            game_state.last_key_fire = 'a';
            break;
        case GDK_KEY_s:
        case GDK_KEY_S:
            game_state.last_key_fire = 's';
            break;
        case GDK_KEY_d:
        case GDK_KEY_D:
            game_state.last_key_fire = 'd';
            break;
            
        // Watergirl controls (arrow keys)
        case GDK_KEY_Up:
            game_state.last_key_water = 'i';
            break;
        case GDK_KEY_Down:
            game_state.last_key_water = 'k';
            break;
        case GDK_KEY_Left:
            game_state.last_key_water = 'j';
            break;
        case GDK_KEY_Right:
            game_state.last_key_water = 'l';
            break;
    }
    
    pthread_mutex_unlock(&game_state.lock);
    return TRUE;
}

// Window destroy callback
void on_window_destroy(GtkWidget *widget, gpointer data) {
    game_state.game_over = TRUE;
    for (int i = 0; i < game_state.hazard_count; i++) {
        game_state.hazards[i].active = FALSE;
        pthread_join(game_state.hazard_threads[i], NULL);
    }
    pthread_mutex_destroy(&game_state.lock);
    gtk_main_quit();
}

// Game loop (runs in GTK main thread)
gboolean game_loop(gpointer data) {
    if (game_state.game_over) {
        return G_SOURCE_REMOVE;
    }
    
    win_lose();
    gtk_widget_queue_draw(game_state.drawing_area);
    
    return G_SOURCE_CONTINUE;
}