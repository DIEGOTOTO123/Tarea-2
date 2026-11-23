/*
 * grid_pathfinding.cpp -- Tarea 2, INF-221 Algoritmos y Complejidad, 2026-2.
 * Autor: Diego Araneda Campusano (rol: 202473538-1)
 */

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;

#ifndef PATH_RESULT_H
#define PATH_RESULT_H
struct PathResult {
    bool found;
    long long score;
    std::string path;
    int remaining_lives;
};
#endif

// Firmas de los algoritmos definidos en algorithms/
PathResult solveBruteForce(int n, int m, int v, int p,
                          const std::vector<std::vector<int>>& types,
                          const std::vector<std::vector<long long>>& values);

PathResult solveDP(int n, int m, int v, int p,
                   const std::vector<std::vector<int>>& types,
                   const std::vector<std::vector<long long>>& values);

PathResult solveGreedy(int n, int m, int v, int p,
                       const std::vector<std::vector<int>>& types,
                       const std::vector<std::vector<long long>>& values);

// ---------------------------------------------------------------------------
// Contador de memoria de heap
// ---------------------------------------------------------------------------
static size_t g_actual = 0;
static size_t g_pico = 0;

__attribute__((noinline)) void* operator new(size_t sz) {
    void* raw = std::malloc(sz + 16);
    if (!raw) throw std::bad_alloc();
    *static_cast<size_t*>(raw) = sz;
    g_actual += sz;
    if (g_actual > g_pico) g_pico = g_actual;
    return static_cast<char*>(raw) + 16;
}

__attribute__((noinline)) void operator delete(void* p) noexcept {
    if (!p) return;
    void* raw = static_cast<char*>(p) - 16;
    g_actual -= *static_cast<size_t*>(raw);
    std::free(raw);
}

void operator delete(void* p, size_t) noexcept {
    operator delete(p);
}

// ---------------------------------------------------------------------------
// Lectura y validación de instancias
// ---------------------------------------------------------------------------
struct GridInstance {
    int n = 0;
    int m = 0;
    int v = 0;
    int p = 0;
    std::vector<std::vector<int>> types;
    std::vector<std::vector<long long>> values;
};

static bool loadGrid(const fs::path& filepath, GridInstance& grid) {
    std::ifstream f(filepath);
    if (!f) return false;
    if (!(f >> grid.n >> grid.m >> grid.v >> grid.p)) return false;

    grid.types.assign(grid.n, std::vector<int>(grid.m, 0));
    grid.values.assign(grid.n, std::vector<long long>(grid.m, 0));

    for (int i = 0; i < grid.n; ++i) {
        for (int j = 0; j < grid.m; ++j) {
            if (!(f >> grid.types[i][j])) return false;
        }
    }
    for (int i = 0; i < grid.n; ++i) {
        for (int j = 0; j < grid.m; ++j) {
            if (!(f >> grid.values[i][j])) return false;
        }
    }
    return true;
}

static bool validateResult(const GridInstance& grid, const PathResult& res) {
    if (!res.found) return true;
    if (static_cast<int>(res.path.size()) != grid.n + grid.m - 2) return false;

    int i = 0, j = 0, lives = grid.v;
    long long score = grid.values[0][0];

    for (char move : res.path) {
        if (move == 'R') {
            ++j;
        } else if (move == 'D') {
            ++i;
        } else {
            return false;
        }
        if (i >= grid.n || j >= grid.m) return false;

        int cell_type = grid.types[i][j];
        if (cell_type == 0) return false;  // Muralla
        if (cell_type == -1) {             // Mina
            --lives;
            score -= grid.p;
            if (lives <= 0) return false;
        } else {
            score += grid.values[i][j];
        }
    }
    return i == grid.n - 1 && j == grid.m - 1 &&
           score == res.score && lives == res.remaining_lives;
}

struct Measurement {
    PathResult res;
    double ms = 0.0;
    size_t mem_bytes = 0;
};

template <class Func>
static Measurement measure(Func fn, int reps) {
    Measurement m;
    for (int k = 0; k < reps; ++k) {
        size_t base = g_actual;
        g_pico = g_actual;
        auto t0 = std::chrono::steady_clock::now();
        PathResult r = fn();
        auto t1 = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        if (k == 0) {
            m.res = r;
            m.ms = elapsed_ms;
            m.mem_bytes = g_pico - base;
        } else {
            m.ms = std::min(m.ms, elapsed_ms);
        }
    }
    return m;
}

static void writeOutput(const fs::path& filepath, const PathResult& res) {
    std::ofstream f(filepath);
    if (!res.found) {
        f << "SIN SOLUCION\n";
        return;
    }
    f << res.score << "\n" << res.path << "\n" << res.remaining_lives << "\n";
}

int main(int argc, char* argv[]) {
    fs::path in_dir  = argc > 1 ? argv[1] : "data/grid_input";
    fs::path out_dir = argc > 2 ? argv[2] : "data/grid_output";
    fs::path med_dir = argc > 3 ? argv[3] : "data/measurements";
    fs::create_directories(out_dir);
    fs::create_directories(med_dir);

    std::vector<fs::path> files;
    if (fs::exists(in_dir)) {
        for (const auto& entry : fs::directory_iterator(in_dir)) {
            if (entry.path().extension() == ".txt") {
                files.push_back(entry.path());
            }
        }
    }

    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        auto key = [](const fs::path& p) {
            std::string s = p.stem().string();
            int n = 0, m = 0;
            std::sscanf(s.c_str(), "%d_%d", &n, &m);
            return std::make_tuple(n, m, s);
        };
        return key(a) < key(b);
    });

    std::ofstream med(med_dir / "measurements.csv");
    med << "n,m,size,tipo,dominio,muestra,algoritmo,tiempo_ms,memoria_kb,estado,puntaje,vidas\n";

    const int REPS = 5;
    int errors = 0;

    for (const auto& filepath : files) {
        std::string base = filepath.stem().string();
        std::vector<std::string> parts;
        {
            std::stringstream ss(base);
            std::string tok;
            while (std::getline(ss, tok, '_')) parts.push_back(tok);
        }
        if (parts.size() != 5) {
            std::cerr << "Nombre inesperado: " << base << "\n";
            continue;
        }

        GridInstance grid;
        if (!loadGrid(filepath, grid)) {
            std::cerr << "No se pudo leer " << filepath << "\n";
            ++errors;
            continue;
        }
        long long size = static_cast<long long>(grid.n) * grid.m;

        struct Run {
            std::string name;
            Measurement meas;
        };
        std::vector<Run> runs;

        if (grid.n + grid.m <= 32) {
            runs.push_back({
                "bruteforce",
                measure([&] {
                    return solveBruteForce(grid.n, grid.m, grid.v, grid.p, grid.types, grid.values);
                }, 1)
            });
        }
        runs.push_back({
            "dp",
            measure([&] {
                return solveDP(grid.n, grid.m, grid.v, grid.p, grid.types, grid.values);
            }, REPS)
        });
        runs.push_back({
            "greedy",
            measure([&] {
                return solveGreedy(grid.n, grid.m, grid.v, grid.p, grid.types, grid.values);
            }, REPS)
        });

        const PathResult* r_bf = nullptr;
        const PathResult* r_dp = nullptr;
        const PathResult* r_gr = nullptr;
        for (const auto& run : runs) {
            if (run.name == "bruteforce") r_bf = &run.meas.res;
            else if (run.name == "dp") r_dp = &run.meas.res;
            else if (run.name == "greedy") r_gr = &run.meas.res;
        }

        for (const auto& run : runs) {
            if (!validateResult(grid, run.meas.res)) {
                std::cerr << "[ERROR] camino invalido: " << base << " " << run.name << "\n";
                ++errors;
            }
        }
        if (r_bf && (r_bf->found != r_dp->found || (r_bf->found && r_bf->score != r_dp->score))) {
            std::cerr << "[ERROR] BF != DP en " << base << "\n";
            ++errors;
        }
        if (r_gr->found && (!r_dp->found || r_gr->score > r_dp->score)) {
            std::cerr << "[ERROR] Greedy supera al optimo en " << base << "\n";
            ++errors;
        }

        for (const auto& run : runs) {
            writeOutput(out_dir / (base + "_" + run.name + "_out.txt"), run.meas.res);
            med << parts[0] << "," << parts[1] << "," << size << "," << parts[2] << ","
                << parts[3] << "," << parts[4] << "," << run.name << ","
                << run.meas.ms << "," << (run.meas.mem_bytes / 1024.0) << ","
                << (run.meas.res.found ? "OK" : "SIN SOLUCION") << ","
                << (run.meas.res.found ? std::to_string(run.meas.res.score) : "") << ","
                << (run.meas.res.found ? std::to_string(run.meas.res.remaining_lives) : "") << "\n";
        }

        std::printf("%-28s DP=%lld  greedy=%s%s\n", base.c_str(),
                    r_dp->found ? r_dp->score : 0LL,
                    r_gr->found ? std::to_string(r_gr->score).c_str() : "SIN SOLUCION",
                    r_bf ? "  (BF verificado)" : "");
        std::fflush(stdout);
    }

    std::printf("\nInstancias: %zu | Errores de verificacion: %d\n", files.size(), errors);
    return errors ? 1 : 0;
}