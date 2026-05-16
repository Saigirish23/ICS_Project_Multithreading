#include "B24CH1046_B24CM1021_B24MT1013_B24ME1066_firewater_.h"

GameState game_state;

// Helper function to calculate distance
double calculate_distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Close game after delay
gboolean close_game(gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
   // gtk_window_close(GTK_WINDOW(window));
    return G_SOURCE_REMOVE;
}

// Update progress bars with lives display
void update_progress_bars() {
    // Calculate Fireboy progress
    int fire_gems_collected = game_state.collected_by_fire;
    double fire_gem_progress = game_state.fire_collectibles > 0 ? 
        (double)fire_gems_collected / game_state.fire_collectibles * 0.5 : 0.5;
    
    // Calculate distance to fire exit for Fireboy
    double max_distance = calculate_distance(0, 0, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE);
    double fire_distance = calculate_distance(game_state.fireboy.x, game_state.fireboy.y, 
                                           game_state.fire_exit_x * TILE_SIZE + TILE_SIZE/2, 
                                           game_state.fire_exit_y * TILE_SIZE + TILE_SIZE/2);
    double fire_distance_progress = (1.0 - (fire_distance / max_distance)) * 0.5;
    
    double fire_total_progress = fire_gem_progress + fire_distance_progress;
    
    // Calculate Watergirl progress
    int water_gems_collected = game_state.collected_by_water;
    double water_gem_progress = game_state.water_collectibles > 0 ? 
        (double)water_gems_collected / game_state.water_collectibles * 0.5 : 0.5;
    
    // Calculate distance to water exit for Watergirl
    double water_distance = calculate_distance(game_state.watergirl.x, game_state.watergirl.y, 
                                            game_state.water_exit_x * TILE_SIZE + TILE_SIZE/2, 
                                            game_state.water_exit_y * TILE_SIZE + TILE_SIZE/2);
    double water_distance_progress = (1.0 - (water_distance / max_distance)) * 0.5;
    
    double water_total_progress = water_gem_progress + water_distance_progress;
    
    // Update progress bars
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(game_state.fire_progress), fire_total_progress);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(game_state.water_progress), water_total_progress);
    
    // Set colors and text
    GtkStyleContext *context = gtk_widget_get_style_context(game_state.fire_progress);
    GtkCssProvider *provider = gtk_css_provider_new();
    char fire_css[200];
    snprintf(fire_css, sizeof(fire_css), 
        "progress, trough { min-height: 20px; } "
        "progress { background-color: #ff0000; color: black; font-weight: bold; }");
    gtk_css_provider_load_from_data(provider, fire_css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    char fire_text[20];
    snprintf(fire_text, sizeof(fire_text), "♥️%d", game_state.fireboy.lives);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(game_state.fire_progress), fire_text);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(game_state.fire_progress), TRUE);
    
    context = gtk_widget_get_style_context(game_state.water_progress);
    provider = gtk_css_provider_new();
    char water_css[200];
    snprintf(water_css, sizeof(water_css), 
        "progress, trough { min-height: 20px; } "
        "progress { background-color: #0000ff; color: black; font-weight: bold; }");
    gtk_css_provider_load_from_data(provider, water_css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    char water_text[20];
    snprintf(water_text, sizeof(water_text), "♥️%d", game_state.watergirl.lives);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(game_state.water_progress), water_text);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(game_state.water_progress), TRUE);
}

// Initialize hazards for current level
void init_hazards() {
    srand(time(NULL));
    game_state.hazard_count = game_state.current_level; // Number of hazards equals level number
    
    for (int i = 0; i < game_state.hazard_count; i++) {
        // Find a valid starting position (not on exit or plate)
        int x, y;
        do {
            x = 1 + rand() % (MAP_WIDTH - 2);
            y = 1 + rand() % (MAP_HEIGHT - 2);
        } while (game_state.level_map[y][x] != TILE_GROUND || 
                (x == game_state.fire_exit_x && y == game_state.fire_exit_y) ||
                (x == game_state.water_exit_x && y == game_state.water_exit_y) ||
                (x == game_state.plate.x && y == game_state.plate.y));
        
        game_state.hazards[i].x = x;
        game_state.hazards[i].y = y;
        game_state.hazards[i].move_type = rand() % 2; // 0 for horizontal, 1 for vertical
        
        // Set initial direction based on move type
        if (game_state.hazards[i].move_type == 0) { // Horizontal
            game_state.hazards[i].direction = (rand() % 2) ? HAZARD_RIGHT : HAZARD_LEFT;
        } else { // Vertical
            game_state.hazards[i].direction = (rand() % 2) ? HAZARD_DOWN : HAZARD_UP;
        }
        
        game_state.hazards[i].active = TRUE;
        
        // Create hazard thread for this hazard
        pthread_create(&game_state.hazard_threads[i], NULL, hazard_thread, (void*)(intptr_t)i);
    }
}

// Check pressure plate
void check_pressure_plate() {
    int plate_activated = 0;
    
    if ((game_state.fireboy.x/TILE_SIZE == game_state.plate.x && 
         game_state.fireboy.y/TILE_SIZE == game_state.plate.y) || 
        (game_state.watergirl.x/TILE_SIZE == game_state.plate.x && 
         game_state.watergirl.y/TILE_SIZE == game_state.plate.y)) {
        plate_activated = 1;
        if (!game_state.plate.active) {
            game_state.plate.active = TRUE;
            // Reveal hidden gem
            if (game_state.hidden_gem_index >= 0) {
                game_state.fire_gems[game_state.hidden_gem_index].hidden = FALSE;
                game_state.water_gems[game_state.hidden_gem_index].hidden = FALSE;
            }
        }
    }
    
    if (!plate_activated && game_state.plate.active) {
        game_state.plate.active = FALSE;
        // Hide the gem again
        if (game_state.hidden_gem_index >= 0) {
            game_state.fire_gems[game_state.hidden_gem_index].hidden = TRUE;
            game_state.water_gems[game_state.hidden_gem_index].hidden = TRUE;
        }
    }
}

// Generate random level
void generate_map() {
    strcpy(game_state.game_message, "");
    // Initialize map with borders
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == 0 || y == MAP_HEIGHT-1 || x == 0 || x == MAP_WIDTH-1) {
                game_state.level_map[y][x] = TILE_FENCE;
                game_state.original_map[y][x] = TILE_FENCE;
            } else {
                game_state.level_map[y][x] = TILE_GROUND;
                game_state.original_map[y][x] = TILE_GROUND;
            }
        }
    }

    // Place fire exit
    int x = 1 + rand() % (MAP_WIDTH - 2);
    int y = 1 + rand() % (MAP_HEIGHT - 2);
    game_state.level_map[y][x] = TILE_FIRE_EXIT;
    game_state.original_map[y][x] = TILE_FIRE_EXIT;
    game_state.fire_exit_x = x;
    game_state.fire_exit_y = y;

    // Place water exit at least 5 tiles away from fire exit
    int attempts = 0;
    do {
        x = 1 + rand() % (MAP_WIDTH - 2);
        y = 1 + rand() % (MAP_HEIGHT - 2);
        attempts++;
        if (attempts > 100) break;
    } while (game_state.level_map[y][x] != TILE_GROUND || 
             abs(x - game_state.fire_exit_x) + abs(y - game_state.fire_exit_y) < 5);
    
    game_state.level_map[y][x] = TILE_WATER_EXIT;
    game_state.original_map[y][x] = TILE_WATER_EXIT;
    game_state.water_exit_x = x;
    game_state.water_exit_y = y;

    // Place lava and water tiles
    int available_width = MAP_WIDTH - 2;
    int available_height = MAP_HEIGHT - 2;
    int lava_count = (int)(available_width * available_height * 0.15);
    int water_count = (int)(available_width * available_height * 0.15);

    for (int i = 0; i < lava_count; i++) {
        x = 1 + rand() % available_width;
        y = 1 + rand() % available_height;
        if (game_state.level_map[y][x] == TILE_GROUND) {
            game_state.level_map[y][x] = TILE_LAVA;
            game_state.original_map[y][x] = TILE_LAVA;
        } else {
            i--;
        }
    }

    for (int i = 0; i < water_count; i++) {
        x = 1 + rand() % available_width;
        y = 1 + rand() % available_height;
        if (game_state.level_map[y][x] == TILE_GROUND) {
            game_state.level_map[y][x] = TILE_WATER;
            game_state.original_map[y][x] = TILE_WATER;
        } else {
            i--;
        }
    }

    // Initialize collectibles (always 3 for each character)
    game_state.fire_collectibles = GEM_COUNT;
    game_state.collected_by_fire = 0;
    game_state.water_collectibles = GEM_COUNT;
    game_state.collected_by_water = 0;
    game_state.hidden_gem_index = -1;

    // For levels 2+, add pressure plate with hidden gem
    if (game_state.current_level >= 2) {
        // Add pressure plate
        do {
            x = 1 + rand() % available_width;
            y = 1 + rand() % available_height;
        } while (game_state.level_map[y][x] != TILE_GROUND);
        
        game_state.level_map[y][x] = TILE_PRESSURE_PLATE;
        game_state.original_map[y][x] = TILE_PRESSURE_PLATE;
        game_state.plate.x = x;
        game_state.plate.y = y;
        game_state.plate.active = FALSE;

        // Choose a random position for hidden gem
        do {
            x = 1 + rand() % available_width;
            y = 1 + rand() % available_height;
        } while (game_state.level_map[y][x] != TILE_GROUND);
        
        game_state.hidden_gem_index = 0; // We'll place the gem here in spawn_gems()
    }

    // Initialize hazards (number equal to level number)
    init_hazards();

    // Find spawn positions for players
    spawn_player(&game_state.fireboy);
    spawn_player(&game_state.watergirl);

    // Calculate initial distances for progress bars
    game_state.max_fire_distance = calculate_distance(game_state.fireboy.x, game_state.fireboy.y, 
                                                    game_state.fire_exit_x * TILE_SIZE, 
                                                    game_state.fire_exit_y * TILE_SIZE);
    game_state.max_water_distance = calculate_distance(game_state.watergirl.x, game_state.watergirl.y, 
                                                     game_state.water_exit_x * TILE_SIZE, 
                                                     game_state.water_exit_y * TILE_SIZE);
    
    if (game_state.max_fire_distance == 0) game_state.max_fire_distance = 1;
    if (game_state.max_water_distance == 0) game_state.max_water_distance = 1;
}

// Spawn gems in the level (always 3 for each character)
void spawn_gems() {
    int placed_fire = 0, placed_water = 0;

    // First place the hidden gem if needed (only one hidden gem)
    if (game_state.hidden_gem_index >= 0) {
        int x, y;
        // Find a ground tile for the hidden gem
        do {
            x = 1 + rand() % (MAP_WIDTH - 2);
            y = 1 + rand() % (MAP_HEIGHT - 2);
        } while (game_state.level_map[y][x] != TILE_GROUND);
        
        // Place fire gem
        game_state.fire_gems[0].x = x * TILE_SIZE;
        game_state.fire_gems[0].y = y * TILE_SIZE;
        game_state.fire_gems[0].collected = FALSE;
        game_state.fire_gems[0].hidden = TRUE;
        
        // Place water gem
        game_state.water_gems[0].x = x * TILE_SIZE;
        game_state.water_gems[0].y = y * TILE_SIZE;
        game_state.water_gems[0].collected = FALSE;
        game_state.water_gems[0].hidden = TRUE;
        
        placed_fire++;
        placed_water++;
    }

    // Place remaining gems (total should be 3 for each character)
    while (placed_fire < game_state.fire_collectibles || placed_water < game_state.water_collectibles) {
        int x = 1 + rand() % (MAP_WIDTH - 2);
        int y = 1 + rand() % (MAP_HEIGHT - 2);
        int tile = game_state.level_map[y][x];

        if (tile == TILE_FIRE_EXIT || tile == TILE_WATER_EXIT || tile == TILE_PRESSURE_PLATE) {
            continue;
        }

        if (placed_fire < game_state.fire_collectibles && (tile == TILE_LAVA || tile == TILE_GROUND)) {
            // Don't place on hidden gem position
            gboolean on_hidden_gem = FALSE;
            if (game_state.hidden_gem_index >= 0) {
                int hidden_x = game_state.fire_gems[0].x / TILE_SIZE;
                int hidden_y = game_state.fire_gems[0].y / TILE_SIZE;
                if (x == hidden_x && y == hidden_y) {
                    on_hidden_gem = TRUE;
                }
            }
            
            if (!on_hidden_gem) {
                game_state.fire_gems[placed_fire].x = x * TILE_SIZE;
                game_state.fire_gems[placed_fire].y = y * TILE_SIZE;
                game_state.fire_gems[placed_fire].collected = FALSE;
                game_state.fire_gems[placed_fire].hidden = FALSE;
                placed_fire++;
            }
        } 
        
        if (placed_water < game_state.water_collectibles && (tile == TILE_WATER || tile == TILE_GROUND)) {
            // Don't place on hidden gem position
            gboolean on_hidden_gem = FALSE;
            if (game_state.hidden_gem_index >= 0) {
                int hidden_x = game_state.water_gems[0].x / TILE_SIZE;
                int hidden_y = game_state.water_gems[0].y / TILE_SIZE;
                if (x == hidden_x && y == hidden_y) {
                    on_hidden_gem = TRUE;
                }
            }
            
            if (!on_hidden_gem) {
                game_state.water_gems[placed_water].x = x * TILE_SIZE;
                game_state.water_gems[placed_water].y = y * TILE_SIZE;
                game_state.water_gems[placed_water].collected = FALSE;
                game_state.water_gems[placed_water].hidden = FALSE;
                placed_water++;
            }
        }
    }
}

// Spawn player at random ground position
void spawn_player(Player *player) {
    int spawn_found = 0;
    while (!spawn_found) {
        int rand_x = 1 + rand() % (MAP_WIDTH - 2);
        int rand_y = 1 + rand() % (MAP_HEIGHT - 2);
        if (game_state.level_map[rand_y][rand_x] == TILE_GROUND) {
            player->spawn_x = rand_x * TILE_SIZE;
            player->spawn_y = rand_y * TILE_SIZE;
            player->x = player->spawn_x;
            player->y = player->spawn_y;
            player->has_moved = FALSE;
            spawn_found = 1;
        }
    }
}

// Respawn player at spawn position
void respawn_player(Player *player) {
    player->x = player->spawn_x;
    player->y = player->spawn_y;
    player->has_moved = FALSE;
    player->lives--;
    update_progress_bars();
    
    if (player->lives <= 0) {
        if (player == &game_state.fireboy) {
            strcpy(game_state.game_message, "Fireboy died! Game Over!");
        } else {
            strcpy(game_state.game_message, "Watergirl died! Game Over!");
        }
        game_state.game_over = TRUE;
        g_timeout_add_seconds(2, close_game, game_state.window);
    }
}

// Check if player is on hazard or goal
void check_hazard_or_goal(Player *player) {
    int grid_x = player->x / TILE_SIZE;
    int grid_y = player->y / TILE_SIZE;
    
    if (grid_x < 0 || grid_x >= MAP_WIDTH || grid_y < 0 || grid_y >= MAP_HEIGHT) {
        return;
    }
    
    int tile = game_state.level_map[grid_y][grid_x];
    
    // Check for gem collection
    if (player == &game_state.fireboy) {
        for (int i = 0; i < game_state.fire_collectibles; i++) {
            if (!game_state.fire_gems[i].collected && !game_state.fire_gems[i].hidden &&
                player->x >= game_state.fire_gems[i].x && 
                player->x < game_state.fire_gems[i].x + TILE_SIZE &&
                player->y >= game_state.fire_gems[i].y && 
                player->y < game_state.fire_gems[i].y + TILE_SIZE) {
                game_state.fire_gems[i].collected = TRUE;
                game_state.collected_by_fire++;
                update_progress_bars();
            }
        }
    } else { // watergirl
        for (int i = 0; i < game_state.water_collectibles; i++) {
            if (!game_state.water_gems[i].collected && !game_state.water_gems[i].hidden &&
                player->x >= game_state.water_gems[i].x && 
                player->x < game_state.water_gems[i].x + TILE_SIZE &&
                player->y >= game_state.water_gems[i].y && 
                player->y < game_state.water_gems[i].y + TILE_SIZE) {
                game_state.water_gems[i].collected = TRUE;
                game_state.collected_by_water++;
                update_progress_bars();
            }
        }
    }

    if (player == &game_state.fireboy) {
        if (tile == TILE_WATER && !(grid_x == player->spawn_x/TILE_SIZE && grid_y == player->spawn_y/TILE_SIZE)) {
            respawn_player(player);
        } else if (tile == TILE_FIRE_EXIT) {
            game_state.fire_at_goal = TRUE;
        } else {
            game_state.fire_at_goal = FALSE;
        }
    } else { // watergirl
        if (tile == TILE_LAVA && !(grid_x == player->spawn_x/TILE_SIZE && grid_y == player->spawn_y/TILE_SIZE)) {
            respawn_player(player);
        } else if (tile == TILE_WATER_EXIT) {
            game_state.water_at_goal = TRUE;
        } else {
            game_state.water_at_goal = FALSE;
        }
    }
}

// Load next level
void load_next_level() {
    // Clean up previous hazard threads
    for (int i = 0; i < game_state.hazard_count; i++) {
        game_state.hazards[i].active = FALSE;
        pthread_join(game_state.hazard_threads[i], NULL);
    }
    game_state.hazard_count = 0;

    strcpy(game_state.game_message, "");
    game_state.current_level++;
    game_state.game_over = FALSE;
    
    generate_map();
    spawn_gems();
    spawn_player(&game_state.fireboy);
    spawn_player(&game_state.watergirl);
    
    gtk_widget_queue_draw(game_state.drawing_area);
    update_progress_bars();
}

// Check if level is completed
gboolean level_completed() {
    return game_state.collected_by_fire >= game_state.fire_collectibles && 
           game_state.collected_by_water >= game_state.water_collectibles &&
           game_state.fire_at_goal && game_state.water_at_goal;
}

// Check win/lose conditions
void win_lose() {
    if (game_state.game_over) return;

    // Check if players are on any hazard
    for (int i = 0; i < game_state.hazard_count; i++) {
        if (game_state.fireboy.x/TILE_SIZE == game_state.hazards[i].x && 
            game_state.fireboy.y/TILE_SIZE == game_state.hazards[i].y &&
            !(game_state.fireboy.x == game_state.fireboy.spawn_x && 
              game_state.fireboy.y == game_state.fireboy.spawn_y)) {
            respawn_player(&game_state.fireboy);
        }
        
        if (game_state.watergirl.x/TILE_SIZE == game_state.hazards[i].x && 
            game_state.watergirl.y/TILE_SIZE == game_state.hazards[i].y &&
            !(game_state.watergirl.x == game_state.watergirl.spawn_x && 
              game_state.watergirl.y == game_state.watergirl.spawn_y)) {
            respawn_player(&game_state.watergirl);
        }
    }

    // Check if level is completed
    if (level_completed()) {
        if (game_state.current_level < LEVEL_COUNT) {
            strcpy(game_state.game_message, "Level Complete!");
            gtk_widget_queue_draw(game_state.drawing_area);
            
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(game_state.window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO,
                "Level %d Complete! Play next level?", game_state.current_level);
            
            gtk_window_set_title(GTK_WINDOW(dialog), "Level Complete");
            gint response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            
            if (response == GTK_RESPONSE_YES) {
                load_next_level();
            } else {
                game_state.game_over = TRUE;
                g_timeout_add_seconds(1, close_game, game_state.window);
            }
        } else {
            strcpy(game_state.game_message, "Congratulations! You completed all levels!");
            game_state.game_over = TRUE;
            g_timeout_add_seconds(5, close_game, game_state.window);
        }
    }
    update_progress_bars();
}

// Input thread function
void* input_thread(void* arg) {
    while (!game_state.game_over) {
        usleep(10000);
    }
    return NULL;
}

// Fireboy thread function
void* fireboy_thread(void* arg) {
    while (!game_state.game_over) {
        pthread_mutex_lock(&game_state.lock);
        
        char ch = game_state.last_key_fire;
        if (ch) {
            game_state.last_key_fire = 0;
            int new_x = game_state.fireboy.x;
            int new_y = game_state.fireboy.y;
            
            if (ch == 'w') new_y -= TILE_SIZE;
            else if (ch == 's') new_y += TILE_SIZE;
            else if (ch == 'a') new_x -= TILE_SIZE;
            else if (ch == 'd') new_x += TILE_SIZE;
            
            if (is_valid_move(new_x, new_y)) {
                game_state.fireboy.x = new_x;
                game_state.fireboy.y = new_y;
                game_state.fireboy.has_moved = TRUE;
                check_hazard_or_goal(&game_state.fireboy);
                check_pressure_plate();
            }
        }
        
        pthread_mutex_unlock(&game_state.lock);
        usleep(50000);
    }
    return NULL;
}

// Watergirl thread function
void* watergirl_thread(void* arg) {
    while (!game_state.game_over) {
        pthread_mutex_lock(&game_state.lock);
        
        char ch = game_state.last_key_water;
        if (ch) {
            game_state.last_key_water = 0;
            int new_x = game_state.watergirl.x;
            int new_y = game_state.watergirl.y;
            
            if (ch == 'i') new_y -= TILE_SIZE;
            else if (ch == 'k') new_y += TILE_SIZE;
            else if (ch == 'j') new_x -= TILE_SIZE;
            else if (ch == 'l') new_x += TILE_SIZE;
            
            if (is_valid_move(new_x, new_y)) {
                game_state.watergirl.x = new_x;
                game_state.watergirl.y = new_y;
                game_state.watergirl.has_moved = TRUE;
                check_hazard_or_goal(&game_state.watergirl);
                check_pressure_plate();
            }
        }
        
        pthread_mutex_unlock(&game_state.lock);
        usleep(50000);
    }
    return NULL;
}

// Hazard thread function (one for each hazard)
void* hazard_thread(void* arg) {
    int hazard_index = (int)(intptr_t)arg;
    Hazard *hazard = &game_state.hazards[hazard_index];
    
    while (hazard->active && !game_state.game_over) {
        pthread_mutex_lock(&game_state.lock);
        
        if (hazard->active && !game_state.game_over) {
            // Calculate next position
            int next_x = hazard->x;
            int next_y = hazard->y;
            
            switch (hazard->direction) {
                case HAZARD_UP: next_y--; break;
                case HAZARD_DOWN: next_y++; break;
                case HAZARD_LEFT: next_x--; break;
                case HAZARD_RIGHT: next_x++; break;
            }
            
            // Check if next position is valid (only fences block movement)
            if (game_state.level_map[next_y][next_x] == TILE_FENCE) {
                // Reverse direction
                switch (hazard->direction) {
                    case HAZARD_UP: hazard->direction = HAZARD_DOWN; break;
                    case HAZARD_DOWN: hazard->direction = HAZARD_UP; break;
                    case HAZARD_LEFT: hazard->direction = HAZARD_RIGHT; break;
                    case HAZARD_RIGHT: hazard->direction = HAZARD_LEFT; break;
                }
            } else {
                hazard->x = next_x;
                hazard->y = next_y;
            }
            
            // Force a redraw
            gtk_widget_queue_draw(game_state.drawing_area);
        }
        
        pthread_mutex_unlock(&game_state.lock);
        usleep(HAZARD_MOVE_INTERVAL);
    }
    return NULL;
}

// Check if move is valid
gboolean is_valid_move(int new_x, int new_y) {
    int grid_x = new_x / TILE_SIZE;
    int grid_y = new_y / TILE_SIZE;
    
    if (grid_x < 0 || grid_x >= MAP_WIDTH || grid_y < 0 || grid_y >= MAP_HEIGHT) {
        return FALSE;
    }
    
    if (game_state.level_map[grid_y][grid_x] == TILE_FENCE) {
        return FALSE;
    }
    
    return TRUE;
}