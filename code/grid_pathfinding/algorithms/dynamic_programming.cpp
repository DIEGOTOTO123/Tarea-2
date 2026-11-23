/*
 * dynamic_programming.cpp -- Tarea 2, INF-221 Algoritmos y Complejidad, 2026-2.
 * Autor: <NOMBRE APELLIDO> (rol: <ROL>)
 *
 * PROGRAMACION DINAMICA (bottom-up / tabulacion) para grid pathfinding con
 * murallas y minas.
 *
 * Estado: dp[i][j][k] = maximo puntaje de un camino valido desde (1,1) hasta
 * (i,j) que ha detonado exactamente k minas (0 <= k <= V-1). Las vidas
 * restantes en esa celda son V-k.
 *
 * Recurrencia (c = celda actual, desde la izquierda o desde arriba):
 *   si c es muralla:  dp[c][k] = -inf
 *   si c es normal:   dp[c][k] = g_c + max(dp[arriba][k], dp[izq][k])
 *   si c es mina:     dp[c][k] = -P  + max(dp[arriba][k-1], dp[izq][k-1])   (k >= 1)
 *   caso base:        dp[(1,1)][0] = g_(1,1)
 * Respuesta: max_k dp[(N,M)][k]; en caso de empate se prefiere k menor
 * (mas vidas restantes).
 *
 * Reconstruccion: se guarda para cada estado el movimiento con el que se
 * llego ('R' o 'D'); el k anterior se deduce (k si la celda es normal, k-1 si
 * es mina), por lo que basta 1 byte extra por estado.
 *
 * Complejidad: tiempo O(N*M*V); espacio O(N*M*V) (se necesita la tabla
 * completa para reconstruir el camino).
 *
 * Referencias: Cormen et al., "Introduction to Algorithms", cap. 14
 * (Programacion Dinamica); problema clasico "minimum path sum" con estado
 * extendido por recurso (vidas).
 */
/*
 * dynamic_programming.cpp -- Tarea 2, INF-221 Algoritmos y Complejidad, 2026-2.
 * Autor: Diego Araneda Campusano (rol: 202473538-1)
 */

#include <algorithm>
#include <limits>
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
constexpr int CELL_WALL = 0;
constexpr int CELL_MINE = -1;
}  // namespace

PathResult solveDP(int n, int m, int v, int p,
                   const std::vector<std::vector<int>>& types,
                   const std::vector<std::vector<long long>>& values) {
    PathResult res{false, 0, "", 0};
    if (n <= 0 || m <= 0 || v <= 0) return res;
    if (types[0][0] == CELL_WALL) return res;

    const long long NEG_INF = std::numeric_limits<long long>::min() / 4;
    const int K = v;  // k minas detonadas en [0, v - 1]; vidas restantes = v - k
    const long long penalty = static_cast<long long>(p);

    auto idx = [&](int i, int j, int k) -> size_t {
        return ((static_cast<size_t>(i) * static_cast<size_t>(m) + static_cast<size_t>(j)) *
                static_cast<size_t>(K)) +
               static_cast<size_t>(k);
    };

    std::vector<long long> dp(static_cast<size_t>(n) * static_cast<size_t>(m) * static_cast<size_t>(K), NEG_INF);
    std::vector<char> parent(static_cast<size_t>(n) * static_cast<size_t>(m) * static_cast<size_t>(K), 0);

    int start_dk = (types[0][0] == CELL_MINE) ? 1 : 0;
    if (start_dk >= K) return res;
    dp[idx(0, 0, start_dk)] = (types[0][0] == CELL_MINE) ? -penalty : values[0][0];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (i == 0 && j == 0) continue;
            int cell_type = types[i][j];
            if (cell_type == CELL_WALL) continue;

            int dk = (cell_type == CELL_MINE) ? 1 : 0;
            long long gain = (cell_type == CELL_MINE) ? -penalty : values[i][j];

            for (int k = dk; k < K; ++k) {
                int prev_k = k - dk;
                long long best_prev = NEG_INF;
                char move_used = 0;

                if (j > 0) {
                    long long cand = dp[idx(i, j - 1, prev_k)];
                    if (cand > best_prev) {
                        best_prev = cand;
                        move_used = 'R';
                    }
                }
                if (i > 0) {
                    long long cand = dp[idx(i - 1, j, prev_k)];
                    if (cand > best_prev) {
                        best_prev = cand;
                        move_used = 'D';
                    }
                }

                if (best_prev > NEG_INF) {
                    dp[idx(i, j, k)] = best_prev + gain;
                    parent[idx(i, j, k)] = move_used;
                }
            }
        }
    }

    int best_k = -1;
    long long best_score = NEG_INF;
    for (int k = 0; k < K; ++k) {
        long long cand = dp[idx(n - 1, m - 1, k)];
        if (cand > best_score) {
            best_score = cand;
            best_k = k;
        }
    }

    if (best_k < 0 || best_score <= NEG_INF / 2) return res;

    res.found = true;
    res.score = best_score;
    res.remaining_lives = v - best_k;

    std::string path;
    path.reserve(static_cast<size_t>(n + m));
    int ci = n - 1, cj = m - 1, ck = best_k;
    while (ci != 0 || cj != 0) {
        char move_used = parent[idx(ci, cj, ck)];
        path.push_back(move_used);
        if (types[ci][cj] == CELL_MINE) --ck;
        if (move_used == 'R') {
            --cj;
        } else {
            --ci;
        }
    }
    std::reverse(path.begin(), path.end());
    res.path = path;
    return res;
}
