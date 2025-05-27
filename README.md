# Car-Lane-Game
1. Abstract
• Developed a 2D car lane avoidance game using OpenGL.
 • Used GLFW for window/input handling and GLAD for OpenGL function loading.
 • Game involves avoiding enemy cars across three lanes.
 • Demonstrates foundational graphics programming and game logic.

2. Theory
• Built using OpenGL 3.3 Core Profile and programmable shaders.
 • GLFW handles window creation and input; GLAD loads OpenGL functions.
 • All game elements are quads with transformation matrices.
 • Collision detection uses Axis-Aligned Bounding Box (AABB).
 • Delta-time is used for smooth, frame-rate-independent movement.
 • Random enemy spawning uses a linear congruential generator.

3. Objectives
• Implement a basic 2D car lane game with player and enemies.
 • Enable player movement, enemy generation, and collision checks.
 • Use shaders for rendering transformed colored quads.
 • Ensure responsive controls and smooth gameplay via delta-time.
 • Establish a base for enhancements like scoring and effects.

4. Tools and Environmental Setup
• Libraries: OpenGL 3.3, GLFW, GLAD.
 • Programming Language: C++.
 • IDE: Visual Studio Code (or equivalent).
 • Compiler: GCC or MSVC.
 • OS: Windows/Linux/macOS (cross-platform compatibility).
 • Setup Steps:
Install GLFW and GLAD.


Configure project with necessary include paths and libraries.


Generate GLAD loader code for OpenGL 3.3.


Compile and run the application.



5. Methodology
• Initialize GLFW and GLAD to create OpenGL context.
 • Write and compile shaders for rendering colored objects.
 • Create a single quad using VAO, VBO, and EBO.
 • Game Logic:
Player movement across 3 lanes using A/D keys.


Randomized enemy spawning with variable speeds.


AABB collision detection between player and enemies.
 • Render loop draws lanes, player, enemies, and state indicator.
 • Input handling includes debouncing to avoid key repeat issues.
 • Main game loop uses delta-time for consistent updates.



6. Code Summary
• GLFW and GLAD setup forms the OpenGL runtime environment.
 • One quad reused for all objects using transformations.
 • Shader program handles transformation and color output.
 • Player moves left/right with keyboard input.
 • Enemies appear in random lanes and fall downward.
 • Collision logic stops game upon overlap detection.
 • Game states (start, playing, over) controlled by flags.

7. Output Images
• Game Start: Green square in top-left corner.
 • During Play: Blue player car, red enemies, white lanes.
 • Game Over: Green square reappears after collision.

8. Result
• Game runs smoothly and performs real-time collision checks.
 • Player input is responsive with smooth transitions.
 • Rendering is consistent across hardware using delta-time.
 • Meets core gameplay goals but lacks advanced features.

9. Description
• A simple 2D avoidance game built with OpenGL.
 • Player controls a blue car across three lanes.
 • Enemies (red) fall down randomly in lanes.
 • Lane dividers (white) and status indicator (green) improve visibility.
 • Reuses a single quad object for all visual elements.

10. Conclusion
• Project meets all primary objectives: gameplay, rendering, input.
 • Limitations identified:
No scoring or difficulty scaling.


Randomization is predictable.


Fixed enemy array limits scalability.


Error handling is minimal.



11. Future Work
• Add a scoring system based on time or dodged cars.
 • Include difficulty progression (e.g., increasing enemy speed).
 • Add visual feedback (e.g., particles or animations).
 • Replace fixed arrays with dynamic containers (e.g., std::vector).
 • Improve randomization using std::random.
 • Add sound effects and background music.
 • Implement UI and pause/resume functionality.

12. References
• OpenGL Documentation
 • GLFW Documentation
 • GLAD Loader Generator
 • Learn OpenGL Tutorials
