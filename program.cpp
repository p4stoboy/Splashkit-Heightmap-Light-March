#include "include/types.h"
#include "splashkit.h"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const double MAX_DISTANCE = 1000;

void render_terrain(const Grid& grid, const LightSource& light) {
    for (int y = 0; y < grid.height; ++y) {
        for (int x = 0; x < grid.width; ++x) {
            Cell cell = get_cell(grid, x, y);

            color terrain_color = height_to_terrain_color(cell.height);

            double luminosity = calculate_cell_luminosity(grid, x, y, light);

            color final_color = blend_color_with_luminosity(terrain_color, luminosity);

            fill_rectangle(final_color, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        }
    }
}

int main() {
    open_window("Terrain Lighting Demo", SCREEN_WIDTH, SCREEN_HEIGHT);

    Grid grid = create_grid(SCREEN_WIDTH / CELL_SIZE, SCREEN_HEIGHT / CELL_SIZE);
    LightSource light = {{SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, 50.0}, 100.0};

    load_font("arial", "arial.ttf");

    while (!quit_requested()) {
        process_events();
        clear_screen(COLOR_BLACK);

        point_2d mouse = mouse_position();
        light.position.x = mouse.x;
        light.position.y = mouse.y;

        if (key_down(UP_KEY)) light.position.z += 1.0;
        if (key_down(DOWN_KEY)) light.position.z -= 1.0;

        // Render the terrain with lighting
        render_terrain(grid, light);

        // Render light height text
        std::string height_text = "Light Height: " + std::to_string(static_cast<int>(light.position.z));
        draw_text(height_text, color_white(), "arial", 20, 10, 10);

        refresh_screen(60);
    }

    return 0;
}
