# 🔥💧 Fireboy & Watergirl — Multithreaded Text-Based Game in C

This is a **Windows-based text-mode game** that mimics the core mechanics of **Fireboy & Watergirl**, developed in **C using POSIX Threads**. The game uses **multithreading** for smooth player input and dynamic hazard behavior, offering multiple levels with collectibles, a life system, and progressive difficulty.

---

## 🎮 Features

- 🔄 **Multithreaded input handling** for Fireboy and Watergirl — responsive simultaneous controls.
- ⚠️ **Dynamic hazards** like moving enemies — also multithreaded.
- ❤️ **Shared lives system** — lose lives on mistakes, game ends when lives hit zero.
- ⭐ **Collectibles** — gather gems before finishing a level.
- 🧩 **Map-based level design** — levels are loaded from external `.txt` files.
- ⏳ **Progress bar and level system** — play through 5 handcrafted levels.
- ✅ **Level completion detection** without requiring pressure plates.
- ❌ **Game Over logic** — game ends when all lives are lost.

---

## 📁 File Structure

```
project_root/
├── fireboy_watergirl.c         // Main source file
├── levels/
│   ├── level1.txt
│   ├── level2.txt
│   ├── level3.txt
│   ├── level4.txt
│   └── level5.txt
└── README.md
```

---

## 🧱 Map Format

Each map file is a **12x7 grid** using characters to represent game elements:

| Symbol | Description            |
|--------|------------------------|
| `#`    | Wall                   |
| `.`    | Empty space            |
| `F`    | Fireboy's starting position |
| `W`    | Watergirl's starting position |
| `G`    | Fireboy's goal         |
| `H`    | Watergirl's goal       |
| `*`    | Collectible gem        |
| `^`    | Hazard/enemy spawn point |

---

## 🧵 Threads Used

- **Input thread**: Reads real-time keyboard input.
- **Fireboy thread**: Moves Fireboy based on input.
- **Watergirl thread**: Moves Watergirl based on input.
- **Hazard threads**: Each hazard runs on its own thread, moving independently.

---

## ⚙️ Requirements

- OS: **Windows**
- Compiler: **GCC (MinGW)** or compatible
- Terminal: **Windows Command Prompt** or any terminal that supports ANSI escape codes

---

## 🛠️ Compilation & Run

1. **Open CMD** and navigate to your project directory.

2. Compile the code:

   ```bash
   gcc fireboy_watergirl.c -o game.exe -lpthread
   ```

3. Run the game:

   ```bash
   game.exe
   ```

---

## 🎮 Controls

| Player     | Action         | Key       |
|------------|----------------|-----------|
| Fireboy    | Move Up        | `W`       |
| Fireboy    | Move Down      | `S`       |
| Fireboy    | Move Left      | `A`       |
| Fireboy    | Move Right     | `D`       |
| Watergirl  | Move Up        | `I`       |
| Watergirl  | Move Down      | `K`       |
| Watergirl  | Move Left      | `J`       |
| Watergirl  | Move Right     | `L`       |
| Global     | Quit game      | `Q` or `q`|

Controls are case-insensitive.

---

## 📜 Game Rules

- Players must navigate to their respective goals.
- You must collect **all gems** before the level is considered complete.
- **Hazards** can move and kill either player.
- Players share a pool of **3 lives**. Losing all lives results in **game over**.
- Game progresses through **5 levels**, each loaded from external files.

---

## 💡 Tips

- Coordination is key! Both players must reach their goals.
- Don’t forget to collect all the gems before heading to the exits.
- Watch out for moving hazards!
- Use a terminal that supports ANSI sequences for the best experience.

---

## 🧼 Future Improvements (Optional Ideas)

- Sound effects or music
- Restart level option
- Timer-based scoring
- Level editor
- High score tracking

---

## 🧑‍💻 Developed With

- **C Language**
- **POSIX Threads (pthreads)**
- **ANSI escape codes for terminal rendering**
- Good ol' patience and debugging :)

---

Let me know if you’d like to generate this file or want it formatted as plain text instead of markdown!
