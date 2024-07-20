#ifndef TYPES_H
#define TYPES_H

#include "splashkit.h"
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

const int CELL_SIZE = 10;
const int MAX_HEIGHT = 80;

struct Cell {
    double height;
    double luminosity;
};

struct Grid {
    std::vector<Cell> cells;
    int width;
    int height;
};

struct Point3D {
    double x, y, z;
};

struct Vector3D {
    double x, y, z;
};

struct LightSource {
    Point3D position;
    double intensity;
};

struct Ray {
    Point3D origin;
    Vector3D direction;
};

struct RayHit {
    Point3D hit_point;
    double distance;
    bool hit_terrain;
};

class PerlinNoise {
private:
    std::vector<int> p;

public:
    PerlinNoise() {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::random_device rd;
        std::mt19937 engine(rd());
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y);

private:
    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y);
};

Grid create_grid(int width, int height);
Cell get_cell(const Grid& grid, int x, int y);
std::vector<RayHit> march_ray(const Grid& grid, const Ray& ray, double max_distance);
std::vector<RayHit> cast_rays(const Grid& grid, Point3D origin, int num_rays_horizontal, int num_rays_vertical, double max_distance);
color height_to_terrain_color(int height);
color blend_color_with_luminosity(color base_color, double luminosity);

// 3D vector operations
Vector3D vector_multiply(const Vector3D& v, double scalar);
Point3D add_vectors(const Point3D& p, const Vector3D& v);

double calculate_cell_luminosity(const Grid& grid, int x, int y, const LightSource& light);
void update_grid_luminosity(Grid& grid, const LightSource& light);

#endif // TYPES_H
