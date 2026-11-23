/*
 * brute_force.cpp -- Tarea 2, INF-221 Algoritmos y Complejidad, 2026-2.
 * Autor: Diego Araneda Campusano (rol: 202473538-1)
 *
 * FUERZA BRUTA para grid pathfinding con murallas y minas.
 *
 * Decisiones de diseno:
 *  - Busqueda exhaustiva por backtracking (DFS): en cada celda se prueban los
 *    movimientos R y D, enumerando los C(N+M-2, N-1) caminos posibles.
 *  - Unica poda permitida por el enunciado: se abandona una rama apenas el
 *    camino se vuelve invalido (pisa una muralla o se queda sin vidas).
 *    No hay cotas sobre el puntaje ni memorizacion.
 *  - El puntaje se acumula incrementalmente durante la recursion; el mejor
 *    camino se guarda solo cuando mejora estrictamente el maximo.
 *  - Complejidad temporal O(C(N+M-2, N-1)) (exponencial); espacio O(N+M)
 *    por la profundidad de la recursion y el buffer del camino.
 *
 * Referencias: Cormen et al., "Introduction to Algorithms" (backtracking /
 * busqueda exhaustiva); apuntes del curso INF-221.
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

struct SearchState {
    int n;
    int m;
    long long p;
    const std::vector<std::vector<int>>* types;
    const std::vector<std::vector<long long>>* values;
    bool found = false;
    long long best_score = 0;
    int best_lives = 0;
    std::string current_path;
    std::string best_path;
};

void dfs(SearchState& st, int i, int j, int lives, long long acc) {
    if (i == st.n - 1 && j == st.m - 1) {
        if (!st.found || acc > st.best_score) {
            st.found = true;
            st.best_score = acc;
            st.best_lives = lives;
            st.best_path = st.current_path;
        }
        return;
    }

    // Derecha (R) y abajo (D)
    for (int dir = 0; dir < 2; ++dir) {
        int ni = i + (dir == 1 ? 1 : 0);
        int nj = j + (dir == 0 ? 1 : 0);
        if (ni >= st.n || nj >= st.m) continue;

        int cell_type = (*st.types)[ni][nj];
        if (cell_type == CELL_WALL) continue;

        long long next_acc = acc;
        int next_lives = lives;

        if (cell_type == CELL_MINE) {
            next_lives = lives - 1;
            if (next_lives <= 0) continue;
            next_acc -= st.p;
        } else {
            next_acc += (*st.values)[ni][nj];
        }

        st.current_path.push_back(dir == 0 ? 'R' : 'D');
        dfs(st, ni, nj, next_lives, next_acc);
        st.current_path.pop_back();
    }
}

}  // namespace

PathResult solveBruteForce(int n, int m, int v, int p,
                          const std::vector<std::vector<int>>& types,
                          const std::vector<std::vector<long long>>& values) {
    PathResult res{false, 0, "", 0};
    if (n <= 0 || m <= 0 || v <= 0) return res;
    if (types[0][0] == CELL_WALL) return res;

    SearchState st;
    st.n = n;
    st.m = m;
    st.p = static_cast<long long>(p);
    st.types = &types;
    st.values = &values;
    st.current_path.reserve(static_cast<size_t>(n + m));

    long long start_score = (types[0][0] == CELL_MINE) ? -st.p : values[0][0];
    int start_lives = (types[0][0] == CELL_MINE) ? v - 1 : v;
    if (start_lives <= 0) return res;

    dfs(st, 0, 0, start_lives, start_score);

    res.found = st.found;
    if (st.found) {
        res.score = st.best_score;
        res.path = st.best_path;
        res.remaining_lives = st.best_lives;
    }
    return res;
}

