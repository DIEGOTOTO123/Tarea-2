/*
 * greedy.cpp -- Tarea 2, INF-221 Algoritmos y Complejidad, 2026-2.
 * Autor: Diego Araneda Campusano (rol: 202473538-1)
 * 
 * GREEDY (voraz) para grid pathfinding con murallas y minas.
 *
 * Implementa exactamente la "Regla greedy" del enunciado: en (i,j) con v
 * vidas, las candidatas son las celdas derecha y abajo que no son murallas y
 * que no son minas cuando v = 1. La ganancia neta es g si la celda es normal
 * y -P si es mina. Se avanza a la candidata de mayor ganancia; en empate se
 * prioriza la derecha. Si no hay candidatas el algoritmo falla (SIN SOLUCION).
 *
 * Decisiones de diseno:
 *  - No hay retroceso ni memoria: cada decision es local e irrevocable.
 *  - Se recorren exactamente N+M-2 pasos como maximo (cada paso avanza R o D).
 *
 * Complejidad: tiempo O(N+M); espacio O(N+M) (solo el camino de salida).
 *
 * Referencias: Cormen et al., "Introduction to Algorithms", cap. 15
 * (Algoritmos Greedy).
 */

#include <string>
#include <vector>

#ifndef PATH_RESULT_H
#define PATH_RESULT_H
struct PathResult {
    bool found;
    long long score;
    std::string path;
    int remaining_lives;
};
#endif

namespace {
constexpr int CELL_NORMAL = 1;
constexpr int CELL_WALL = 0;
constexpr int CELL_MINE = -1;
}  // namespace

PathResult solveGreedy(int n, int m, int v, int p,
                       const std::vector<std::vector<int>>& types,
                       const std::vector<std::vector<long long>>& values) {
    PathResult res{false, 0, "", 0};
    if (n <= 0 || m <= 0 || v <= 0) return res;
    if (types[0][0] == CELL_WALL) return res;

    int i = 0, j = 0, lives = v;
    long long score = (types[0][0] == CELL_MINE) ? -static_cast<long long>(p) : values[0][0];
    if (types[0][0] == CELL_MINE) --lives;
    if (lives <= 0) return res;

    std::string path;
    path.reserve(static_cast<size_t>(n + m));

    while (i != n - 1 || j != m - 1) {
        bool has_r = false, has_d = false;
        long long gain_r = 0, gain_d = 0;

        if (j + 1 < m) {
            int t = types[i][j + 1];
            if (t == CELL_NORMAL) {
                has_r = true;
                gain_r = values[i][j + 1];
            } else if (t == CELL_MINE && lives > 1) {
                has_r = true;
                gain_r = -static_cast<long long>(p);
            }
        }

        if (i + 1 < n) {
            int t = types[i + 1][j];
            if (t == CELL_NORMAL) {
                has_d = true;
                gain_d = values[i + 1][j];
            } else if (t == CELL_MINE && lives > 1) {
                has_d = true;
                gain_d = -static_cast<long long>(p);
            }
        }

        if (!has_r && !has_d) return res;

        bool go_right = has_r && (!has_d || gain_r >= gain_d);
        int ni = i + (go_right ? 0 : 1);
        int nj = j + (go_right ? 1 : 0);

        if (types[ni][nj] == CELL_MINE) --lives;
        score += go_right ? gain_r : gain_d;
        path.push_back(go_right ? 'R' : 'D');
        i = ni;
        j = nj;
    }

    res.found = true;
    res.score = score;
    res.remaining_lives = lives;
    res.path = path;
    return res;
}
