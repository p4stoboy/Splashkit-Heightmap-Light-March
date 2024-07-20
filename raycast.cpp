#include "./include/types.h"
#include <cmath>

Grid create_grid(int width, int height) {
    Grid grid;
    grid.width = width;
    grid.height = height;
    grid.cells.resize(width * height);

    PerlinNoise noise;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double value = noise.noise(x * 0.03, y * 0.03);

            int cell_height = std::max(0, static_cast<int>(std::floor(value * MAX_HEIGHT)));
            grid.cells[y * width + x].height = cell_height;
            grid.cells[y * width + x].luminosity = 0.0;
        }
    }

    return grid;
}

Cell get_cell(const Grid& grid, int x, int y) {
    if (x >= 0 && x < grid.width && y >= 0 && y < grid.height) {
        return grid.cells[y * grid.width + x];
    }
    return {0, 0};
}

Vector3D vector_multiply(const Vector3D& v, double scalar) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

Point3D add_vectors(const Point3D& p, const Vector3D& v) {
    return {p.x + v.x, p.y + v.y, p.z + v.z};
}

double calculate_cell_luminosity(const Grid& grid, int start_x, int start_y, const LightSource& light) {
    double start_world_x = (start_x + 0.5) * CELL_SIZE;
    double start_world_y = (start_y + 0.5) * CELL_SIZE;
    double start_height = static_cast<double>(get_cell(grid, start_x, start_y).height);

    // direction between cell and light along each axis
    double dx = light.position.x - start_world_x;
    double dy = light.position.y - start_world_y;
    double dz = light.position.z - start_height;

    // euclidean distance between cell and light
    double total_distance = std::sqrt(dx*dx + dy*dy + dz*dz);

    // step magnitude per axis
    dx /= total_distance;
    dy /= total_distance;
    dz /= total_distance;

    // initial ray position
    double current_x = start_world_x;
    double current_y = start_world_y;
    double current_z = start_height;

    // step size (half a cell, increase for performance, decrease for fidelity)
    double step_size = CELL_SIZE / 2.0;

    for (double distance_traveled = 0; distance_traveled < total_distance; distance_traveled += step_size) {
        current_x += dx * step_size;
        current_y += dy * step_size;
        current_z += dz * step_size;

        int grid_x = static_cast<int>(current_x / CELL_SIZE);
        int grid_y = static_cast<int>(current_y / CELL_SIZE);

        // check bounds
        if (grid_x < 0 || grid_x >= grid.width || grid_y < 0 || grid_y >= grid.height) {
            break;
        }

        // check intersection with grid at current height
        if (current_z <= get_cell(grid, grid_x, grid_y).height) {
            // luminosity based on distance traveled
            return 0.1 + (1.0 - 0.1) * (distance_traveled / total_distance);
        }
    }

    // no obstacles
    double luminosity = 1.0 - (total_distance / (light.intensity * CELL_SIZE * std::max(grid.width, grid.height)));
    // clamp luminosity
    return std::max(0.1, std::min(1.0, luminosity));
}

void update_grid_luminosity(Grid& grid, const LightSource& light) {
    for (int y = 0; y < grid.height; ++y) {
        for (int x = 0; x < grid.width; ++x) {
            grid.cells[y * grid.width + x].luminosity = calculate_cell_luminosity(grid, x, y, light);
        }
    }
}

color luminosity_to_color(double luminosity) {
    int value = static_cast<int>(luminosity * 255);
    return rgba_color(value, value, value, 255);
}

color blend_color_with_luminosity(color base_color, double luminosity) {
    int r = static_cast<int>(red_of(base_color) * luminosity);
    int g = static_cast<int>(green_of(base_color) * luminosity);
    int b = static_cast<int>(blue_of(base_color) * luminosity);
    return rgba_color(r, g, b, 255);
}

color height_to_terrain_color(int height) {
    if (height < 5) {
        return rgba_color(118, 149, 250, 255); // Water
    } else if (height < 20) {
        return rgba_color(252, 238, 113, 255); // Sand
    } else if (height < 50) {
        return rgba_color(124, 100, 54, 255); // Rocks
    } else {
        return rgba_color(105, 219, 88, 255); // Grass
    }
}
