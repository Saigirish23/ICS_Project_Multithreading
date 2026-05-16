#ifndef B24CH1046_B24CM1021_B24MT1013_B24ME1066_FIREWATER_H
#define B24CH1046_B24CM1021_B24MT1013_B24ME1066_FIREWATER_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// Game constants
#define TILE_SIZE 100
#define MAP_WIDTH 15
#define MAP_HEIGHT 9
#define GEM_COUNT 3
#define PROGRESS_BAR_HEIGHT 20
#define MAX_LIVES 3
#define LEVEL_COUNT 5
#define HAZARD_MOVE_INTERVAL 300000  // microseconds between hazard movements

// Tile types
#define TILE_FENCE 0
#define TILE_LAVA 1
#define TILE_WATER 2
#define TILE_GROUND 3
#define TILE_FIRE_EXIT 4
#define TILE_WATER_EXIT 5
#define TILE_HAZARD 6
#define TILE_PRESSURE_PLATE 7

// Hazard directions
typedef enum {
    HAZARD_UP,
    HAZARD_DOWN,
    HAZARD_LEFT,
    HAZARD_RIGHT
} HazardDirection;

// Hazard struct
typedef struct {
    int x, y; // Grid positions
    HazardDirection direction;
    int move_type; // 0 for horizontal, 1 for vertical
    gboolean active;
} Hazard;

// Pressure Plate struct
typedef struct {
    int x, y;
    gboolean active;
} PressurePlate;

// Gem struct
typedef struct {
    int x, y;
    gboolean collected;
    gboolean hidden; // For hidden gems
} Gem;

// Player struct
typedef struct {
    int x, y;
    int spawn_x, spawn_y;
    GdkPixbuf *sprite;
    gboolean has_moved;
    int lives;
} Player;

// Game state
typedef struct {
    int level_map[MAP_HEIGHT][MAP_WIDTH];
    int original_map[MAP_HEIGHT][MAP_WIDTH];
    Player fireboy, watergirl;
    int fire_exit_x, fire_exit_y;
    int water_exit_x, water_exit_y;
    int fire_collectibles, water_collectibles;
    int collected_by_fire, collected_by_water;
    gboolean fire_at_goal, water_at_goal;
    int max_fire_distance, max_water_distance;
    Hazard hazards[LEVEL_COUNT]; // Array for hazards (max 5)
    int hazard_count; // Current number of hazards
    PressurePlate plate;
    gboolean game_over;
    char game_message[100];
    int current_level;
    GtkWidget *drawing_area;
    GtkWidget *fire_progress;
    GtkWidget *water_progress;
    GtkWidget *window;
    pthread_mutex_t lock;
    char last_key_fire;
    char last_key_water;
    GdkPixbuf *fire_gem_image;
    GdkPixbuf *water_gem_image;
    GdkPixbuf *lava_tile;
    GdkPixbuf *water_tile;
    GdkPixbuf *ground_tile;
    GdkPixbuf *fire_exit_tile;
    GdkPixbuf *water_exit_tile;
    GdkPixbuf *fence_tile;
    GdkPixbuf *hazard_image;
    GdkPixbuf *pressure_plate_image;
    Gem fire_gems[GEM_COUNT];
    Gem water_gems[GEM_COUNT];
    int hidden_gem_index; // Index of the hidden gem
    pthread_t hazard_threads[LEVEL_COUNT]; // Threads for each hazard
} GameState;

extern GameState game_state;

// Function prototypes
void update_progress_bars();
void init_hazards();
void generate_map();
void spawn_gems();
void spawn_player(Player *player);
void respawn_player(Player *player);
void check_pressure_plate();
gboolean is_valid_move(int new_x, int new_y);
void win_lose();
void check_hazard_or_goal(Player *player);
gboolean level_completed();
void* input_thread(void* arg);
void* fireboy_thread(void* arg);
void* watergirl_thread(void* arg);
void* hazard_thread(void* arg);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void on_window_destroy(GtkWidget *widget, gpointer data);
void on_start(GtkApplication *app, gpointer user_data);
gboolean game_loop(gpointer data);
gboolean close_game(gpointer data);
void load_next_level();

#endif // B24CH1046_B24CM1021_B24MT1013_B24ME1066_FIREWATER_H