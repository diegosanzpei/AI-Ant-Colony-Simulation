Camera controls (spherical camera):
  - Q/E Keys: Zoom out/zoom in
  - A/D: Rotate left/right around center point
  - S/W: Rotate down/up around center point

Options window (ImGui):
  - Toggle Fullscreen button: Change between windowed and fullscreen mode
  - Level Edit options:
      - Nest mode: Left click on level to place the nest (only first time) and start the simulation.
      - Food mode: Hold left mouse button to drop cubes of food around the level.
      - Wall mode: Hold left mouse button to add walls, hold right mouse button to remove walls.

Simulation parameters (ImGui):
  - Number Of Ants: Add/remove ants from the simulation (this will reset the simulation, removing all walls and food already placed!).
                    Also, if frame rate drops, reduce the number of ants.
  - Pause simulation checkbox: When checked, everything will be paused, but you can still place food/walls.
  - Reset simulation: Eliminate all ants, nest, food, walls etc and restart all the ants' properties.
  - Drop Pheromone Time: The time interval between each pheromone drop of ants.
  - Pheromone Decision Time: The time interval between each decision that an ant takes of which direction to follow
                             based on the pheromone concentration ahead.
  - Ant FOV Radius: The radius of the ants' FOV.
  - Pheromone Time: The time it takes for a pheromone dropped to completely evaporate.

IMPORTANT: When adjusting the slider of number of ants, this will reset the simulation (remove walls and food already placed),
so be careful!.

IMPORTANT: The ImGui simulation parameters are available for you to play around. However, it may not work perfectly for certain values,
so have that in mind, and know that it will work almost always with the default parameters.