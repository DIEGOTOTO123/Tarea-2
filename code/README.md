# Documentación

**Autor:** Diego Araneda Campusano (rol: `202473538-1`)  
**Asignatura:** INF-221 Algoritmos y Complejidad — Tarea 2 (2026-2)

## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

## Requisitos del entorno

* **Compilador C++:** `g++` con soporte para C++17 (`-std=c++17`) y `make`.
* **Python:** Python 3.10+ con los paquetes `numpy`, `pandas` y `matplotlib`.
  * En sistemas Ubuntu/WSL recientes (PEP 668), se recomienda crear y activar un entorno virtual antes de instalar dependencias y ejecutar los scripts:
    ```bash
    python3 -m venv venv
    source venv/bin/activate
    pip install numpy pandas matplotlib
    ```

## Grid pathfinding

Algoritmos: Fuerza Bruta, Programación Dinámica, Greedy.

Problema: en una grilla de `N x M` con murallas (`0`), minas (`-1`) y celdas normales (`1`) con valor `g`, encontrar el camino de `(1,1)` a `(N,M)` (solo movimientos `R` y `D`) de máximo puntaje con `V` vidas (una mina consume una vida y resta `P`; el camino es válido si detona a lo más `V-1` minas).

| Algoritmo | Archivo | Tiempo | Espacio |
|---|---|---|---|
| Fuerza bruta (backtracking, única poda: camino inválido) | `algorithms/brute_force.cpp` | `O(C(N+M-2, N-1))` | `O(N+M)` |
| Programación dinámica (bottom-up, estado `(i, j, minas detonadas)`) | `algorithms/dynamic_programming.cpp` | `O(N·M·V)` | `O(N·M·V)` |
| Greedy (regla local del enunciado, derecha gana empates) | `algorithms/greedy.cpp` | `O(N+M)` | `O(N+M)` |

Las decisiones de diseño y referencias están al inicio de cada archivo fuente.

### Programa principal

Archivos: `grid_pathfinding/grid_pathfinding.cpp`, `grid_pathfinding/makefile`.

```bash 
source venv/bin/activate  # (opcional) activa el entorno virtual de Python en caso de que se use
```

```bash
cd code/grid_pathfinding
make grids                # genera las grillas en data/grid_input/ con semilla 2026
make                      # compila (g++ -O2 -std=c++17)
make run                  # ejecuta los 3 algoritmos sobre todas las instancias
make plots                # genera los gráficos en data/plots/ y tablas resumen
```

o directamente: `./grid_pathfinding [dir_entrada] [dir_salidas] [dir_mediciones]`
(por defecto `data/grid_input data/grid_output data/measurements`).

Qué hace:

- Lee cada `{n}_{m}_{t}_{d}_{s}.txt` y ejecuta DP y Greedy en todas las instancias; Fuerza Bruta solo si `N + M <= 32`.
- Escribe `data/grid_output/{n}_{m}_{t}_{d}_{s}_{bruteforce|dp|greedy}_out.txt` (puntaje, camino, vidas restantes; o `SIN SOLUCION`).
- Escribe `data/measurements/measurements.csv` con tiempo [ms], memoria [KiB], estado, puntaje y vidas por instancia y algoritmo.
- **Tiempo:** `steady_clock`; Fuerza Bruta 1 ejecución, DP y Greedy el mínimo de 5 repeticiones.
- **Memoria:** pico de heap adicional por algoritmo (se reemplazan `operator new/delete`; la grilla de entrada no se cuenta).
- **Verificaciones automáticas** (el programa termina con código 1 si alguna falla): cada camino se re-simula y debe reproducir puntaje y vidas; puntaje(Fuerza Bruta) = puntaje(DP); puntaje(Greedy) <= puntaje(DP).

### Scripts

- `scripts/grid_generator.py`: genera las grillas de prueba (`python3 scripts/grid_generator.py --seed 2026`; requiere `numpy`).
- `scripts/plot_generator.py`: lee `data/measurements/measurements.csv` y guarda en `data/plots/` (y sincroniza en `report/figures/`) los PNG de tiempo vs tamaño, memoria vs tamaño, brecha `Δ` y tasa de falla de Greedy; además escribe tablas `resumen_*.csv` en `data/measurements/` (requiere `pandas`, `matplotlib`). Uso: `make plots`.

