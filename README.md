# **🔥💧 Fireboy & Watergirl — Multithreaded Graphical Game in C**

This is a **multithreaded graphical game** inspired by **Fireboy & Watergirl**, developed in **C using GTK for GUI** and **POSIX Threads** for multithreading. Players control Fireboy and Watergirl simultaneously to navigate levels, collect gems, avoid hazards, and reach their respective exits. Cooperation is key to completing levels!

---

## 🎮 **Features**

- 🔄 **Multithreaded player controls** for smooth simultaneous movement of Fireboy and Watergirl.
- ⚠️ **Dynamic hazards** (e.g., moving obstacles) controlled by independent threads.
- ⭐ **Collectibles** — gather gems before reaching the exits.
- 🧩 **Randomized map generation** — each level has unique layouts with obstacles, exits, and collectibles.
- ⏳ **Progress bars** — track gem collection and proximity to exits.
- ❤️ **Lives system** — lose lives upon mistakes; game ends when lives hit zero.
- ✅ **Level completion detection** — advance through progressively harder levels.

---

## 📁 **File Structure**

```
project_root/
├── fireboy_watergirl.c         // Main source file
├── assets/
│   ├── FireBoy.png
│   ├── WaterGirl.png
│   ├── lava.png
│   ├── water.png
│   ├── ground.png
│   ├── fireexit.png
│   ├── waterexit.png
│   ├── firegem.png
│   ├── watergem.png
│   ├── fence.png
│   ├── hazard.png
│   └── barrier.png
└── README.md
```

---

## ⚙️ **Requirements**

- **Operating System**: Linux or any system supporting GTK.
- **Dependencies**:
  - GTK 3 or higher.
  - POSIX Threads (pthreads).
- **Compiler**: GCC.

---

## 🛠️ **Compilation & Run**

1. Open a terminal in the project directory.
2. Compile the game using:
   ```bash
   gcc fireboy_watergirl.c -o fireboy_watergirl `pkg-config --cflags --libs gtk+-3.0` -lpthread -lm
   ```
3. Run the game:
   ```bash
   ./fireboy_watergirl
   ```

Ensure that all required image files (e.g., `FireBoy.png`, `WaterGirl.png`, etc.) are located in the `assets` directory.

---

## 🎮 **Controls**

| Player     | Action         | Key       |
|------------|----------------|-----------|
| Fireboy    | Move Up        | `W`       |
| Fireboy    | Move Down      | `S`       |
| Fireboy    | Move Left      | `A`       |
| Fireboy    | Move Right     | `D`       |
| Watergirl  | Move Up        | Up Arrow (`↑`)
| Watergirl  | Move Down      | Down Arrow (`↓`)
| Watergirl  | Move Left      | Left Arrow (`←`)
| Watergirl  | Move Right     | Right Arrow (`→`)

---

## 📜 **Game Rules**

1. Players must navigate to their respective exits (Fireboy → Fire Exit; Watergirl → Water Exit).
2. Collect all gems before heading to the exits.
3. Avoid hazards:
   - Fireboy cannot touch water tiles or moving hazards.
   - Watergirl cannot touch lava tiles or moving hazards.
4. Players share a pool of lives (3 per player). Losing all lives results in a game over.
5. Complete all levels to win the game!

---

## 🧩 **Gameplay Instructions**

1. Start the game by running the executable (`./fireboy_watergirl`).
2. Use the controls to move Fireboy and Watergirl simultaneously.
3. Navigate through randomized maps featuring:
   - Lava and water tiles as obstacles.
   - Gems as collectibles.
   - Pressure plates to deactivate barriers (in advanced levels).
4. Progress bars at the bottom of the screen show:
   - Gem collection progress.
   - Proximity to exits.
5. Complete all levels (5 total) to win!

---

## 💡 **Tips**

- Coordinate movements between Fireboy and Watergirl carefully!
- Keep an eye on hazards — they move dynamically!
- Collect all gems before heading to exits; skipping gems prevents level completion.

---

## 🧑‍💻 **Developed Using**

- **C Language**
- **GTK Library** for graphical rendering.
- **POSIX Threads** for multithreading.

Enjoy playing! 🎮
