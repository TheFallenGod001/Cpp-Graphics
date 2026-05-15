# Cpp-Graphics

Two SDL3 graphics projects: an interactive orthogonal line drawing tool and a 2-link robotic arm inverse kinematics visualizer.

## Projects

### Line Generation

Draw grid-aligned line paths with arrow keys. Every segment is horizontal or vertical, so you create orthogonal mazes and geometric designs.

**Controls:**

* RIGHT — extend line to the right
* LEFT — extend line to the left / switch to horizontal
* UP — extend line upward / switch to vertical
* DOWN — extend line downward

Start with a point and press arrow keys. Each perpendicular arrow creates a corner and starts a new segment. The path stays on screen and you can keep building. Bounds checks prevent you from leaving the 800×800 window.

**Code highlights:**

* `lineNodes` struct stores corners, segment lengths, and directions
* `drawLines()` renders the accumulated path via `SDL_RenderLines`
* Real-time keyboard polling for responsive control

### 2-Link Robotic Arm

Visualize a 2-DOF (two-joint) planar arm reaching for a target point. The program solves inverse kinematics in real time and animates the arm following a curved path.

**Interactive:**

* **Drag upper red line** — move the shoulder (pivot point) up/down
* **Drag lower red line** — move the target (end effector) up/down  
* **Drag vertical slider** — move the target left/right

The arm recalculates joint angles instantly and redraws. When idle, it plays back a pre-recorded semicircular path.

**What you see:**

* Two red horizontal lines (constraints)
* White lines (the arm segments)
* Yellow arcs (joint angles)
* Brown line (ground reference)

**Code highlights:**

* `FPointStack` and `FPointBuffer`: Custom data structures for storing trajectory points
* Inverse kinematics using law of cosines: solves for shoulder angle (theta), elbow bend (alpha), wrist angle (beta)
* Numerical integration (`numericalIntTrap`) for arc length calculations
* `genSemi()` generates a semicircular dome path
* `followPath()` samples the path at a given speed/FPS

## Building

Both projects use SDL3 and require:
* SDL3
* SDL3_ttf (for text in the arm project)

### On Linux:
```bash
# Install dependencies
sudo apt install libsdl3-dev libsdl3-ttf-dev

# Build
g++ -std=c++20 -I/path/to/SDL3/include \
    Line\ Generation/main.cpp \
    -o line_gen \
    -L/path/to/SDL3/lib -lSDL3

g++ -std=c++20 -I/path/to/SDL3/include \
    2-Link-Robotic-Arm/main.cpp \
    -o robotic_arm \
    -L/path/to/SDL3/lib -lSDL3 -lSDL3_ttf
```

### On Windows / macOS:
Adjust include/library paths as needed for your SDL3 installation.

## Running

```bash
./line_gen       # Draw orthogonal paths
./robotic_arm    # Interact with the arm
```

## Notes

**Line Generation:**
* Uses a circular queue (`FPointBuffer`) for potentially streaming drawing commands (not fully used in current version)
* The `lineNodes` struct could be expanded to support curves or varying widths

**2-Link Arm:**
* Assumes two equal-length links (300 units each) — edit `l1`, `l2` in main to change
* The shoulder is fixed at (400, upperLimit)
* Numerical integration uses trapezoidal rule with 300 steps (adjustable via `numericalIntTrap` calls)
* Central difference formula for derivatives (`centralDiff`) helps compute arc length
* Joint angle calculations use `std::acos`, `std::atan2` — clamped to [-1, 1] to avoid NaN from floating point errors

**Both:**
* 800×800 window, 120 FPS target with VSync
* Event-driven (respond to window close immediately)
* Uses lambda captures for functional programming in several places

## Potential improvements

* **Line Generation**: Add undo/redo, save/load paths, variable line thickness, curves
* **Robotic Arm**: Add a third joint (3-DOF), multiple arms, trajectory recording/playback, workspace visualization, singularity detection
* Both: Migrate to a more modern graphics API (Vulkan, Metal) if performance becomes needed
