# **Fireboy & Watergirl Game - README**

## **Introduction**
Welcome to the "Fireboy & Watergirl" game! This is a two-player cooperative puzzle platformer where you control Fireboy and Watergirl to navigate through levels, avoid hazards, collect gems, and reach their respective exits. Fireboy cannot touch water, and Watergirl cannot touch lava, so teamwork is essential to win!

This game is implemented in C using the GTK library for graphical rendering and pthreads for multithreading.

---

## **Game Objective**
- Fireboy must collect fire gems and reach the fire exit while avoiding water hazards.
- Watergirl must collect water gems and reach the water exit while avoiding lava hazards.
- Both players must work together to activate pressure plates, disable barriers, and complete all levels.

---

## **System Requirements**
1. **Operating System**: Linux (or any system supporting GTK).
2. **Dependencies**:
   - GTK 3 or higher.
   - Pthreads library.
3. **Compiler**: GCC.

---

## **How to Compile the Game**
1. Open a terminal in the directory containing the source file (`game.c`).
2. Run the following command to compile the program:
   ```bash
   gcc game.c -o fireboy_watergirl `pkg-config --cflags --libs gtk+-3.0` -lm -lpthread
   ```
3. Ensure that all required image files (e.g., `FireBoy.png`, `WaterGirl.png`, `lava.png`, etc.) are in the same directory as the compiled program.

---

## **How to Run the Game**
1. After compiling, run the game using:
   ```bash
   ./fireboy_watergirl
   ```
2. The game window will open, displaying the grid-based map.

---

## **Game Controls**
### **Fireboy (Player 1)**
- Move up: `W`
- Move down: `S`
- Move left: `A`
- Move right: `D`

### **Watergirl (Player 2)**
- Move up: Up Arrow (`↑`)
- Move down: Down Arrow (`↓`)
- Move left: Left Arrow (`←`)
- Move right: Right Arrow (`→`)

---

## **Gameplay Instructions**
1. **Start**:
   - The game begins with Fireboy and Watergirl placed randomly on the map.
   - Collect all gems before heading to your respective exits.

2. **Hazards**:
   - Fireboy must avoid water tiles.
   - Watergirl must avoid lava tiles.
   - Both players must avoid moving hazards.

3. **Pressure Plates & Barriers**:
   - Step on pressure plates to deactivate barriers blocking your path.

4. **Lives**:
   - Each player starts with 3 lives.
   - Losing all lives ends the game.

5. **Progress Bars**:
   - Track your progress on collecting gems and reaching exits via progress bars at the bottom of the screen.

6. **Winning a Level**:
   - Both players must collect their respective gems and reach their exits to complete a level.

7. **Advancing Levels**:
   - Complete a level to unlock the next one.
   - There are 5 levels in total, with increasing difficulty.

8. **Losing Conditions**:
   - If either player loses all lives or cannot reach their exit, you lose the game.

---

## **Game Features**
1. **Randomized Levels**: Each level generates a unique map layout.
2. **Multithreading**: Smooth gameplay with separate threads for player movement, hazard movement, and input handling.
3. **Graphics**: Visual representation of tiles (lava, water, ground), players, gems, hazards, and barriers using GTK's drawing area.
4. **Cooperative Gameplay**: Requires teamwork between Fireboy and Watergirl for success.

---

## **Troubleshooting**
1. If you encounter issues with missing images:
   - Ensure that all required image files are present in the directory where you run the game.
2. If GTK is not installed:
   - Install GTK using your package manager (e.g., `sudo apt install libgtk-3-dev` on Ubuntu).

---

## **Credits**
This game was developed as a fun project inspired by classic platformer games. Enjoy playing! 🎮

--- 

Feel free to modify or extend this README as needed!

Citations:
[1] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/52898687/8f773fbc-acef-4964-968e-6f8c24de5607/paste.txt

---
Answer from Perplexity: pplx.ai/share
