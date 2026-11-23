"""
Script para generar gráficos a partir de las mediciones experimentales.
Lee los resultados desde data/measurements/ y guarda los gráficos en data/plots/
(y copia una versión en report/figures/ si el directorio existe).

Autor: Diego Araneda Campusano (rol: 202473538-1) -- Tarea 2, INF-221, 2026-2.

Métricas graficadas:
  - Tiempo de ejecución vs tamaño (N * M) para cada algoritmo y tipo de grilla.
  - Uso de memoria vs tamaño para cada algoritmo.
  - Brecha de puntaje Delta = s(OPT) - s(Greedy) para casos donde greedy encuentra solución.
  - Tasa de falla de Greedy (% de casos con 'SIN SOLUCION') según tipo de grilla y dominio.

Decisiones de diseño:
  - Paleta de Okabe-Ito (segura para daltonismo) y un marcador distinto por serie,
    de modo que la identidad no dependa solo del color.
  - Escalas log-log para tiempo y memoria (el tamaño abarca varios órdenes de magnitud).
  - Opt (referencia exacta) = programación dinámica; coincide con fuerza bruta
    en todas las instancias donde ambas corren (verificado en el programa principal).
  - Además de las figuras, se escriben tablas resumen (resumen_*.csv) en data/measurements/.

Uso:  python3 plot_generator.py
"""

import os
import shutil

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

BASE = os.path.dirname(os.path.abspath(__file__))
MEASUREMENTS_DIR = os.path.join(BASE, "..", "data", "measurements")
PLOTS_DIR = os.path.join(BASE, "..", "data", "plots")
REPORT_FIGURES_DIR = os.path.join(BASE, "..", "..", "report", "figures")

ALGOS = ["bruteforce", "dp", "greedy"]
ETIQUETA = {"bruteforce": "Fuerza bruta", "dp": "Prog. dinámica", "greedy": "Greedy"}
COLOR = {"bruteforce": "#D55E00", "dp": "#0072B2", "greedy": "#009E73"}
MARCA = {"bruteforce": "s", "dp": "o", "greedy": "^"}
TIPOS = ["libre", "murallas", "minas", "mixta"]
COLOR_TIPO = {"libre": "#0072B2", "murallas": "#D55E00", "minas": "#009E73", "mixta": "#CC79A7"}
MARCA_TIPO = {"libre": "o", "murallas": "s", "minas": "^", "mixta": "D"}
DOMINIOS = ["D10", "D1000"]


def estilo():
    plt.rcParams.update({
        "figure.dpi": 150,
        "savefig.dpi": 150,
        "axes.spines.top": False,
        "axes.spines.right": False,
        "axes.grid": True,
        "grid.alpha": 0.25,
        "grid.linewidth": 0.6,
        "axes.titlesize": 10,
        "axes.labelsize": 9,
        "legend.fontsize": 8,
        "legend.frameon": False,
        "font.size": 9,
        "lines.linewidth": 1.6,
        "lines.markersize": 5,
    })


def cargar():
    ruta = os.path.join(MEASUREMENTS_DIR, "measurements.csv")
    d = pd.read_csv(ruta)
    d["ok"] = d["estado"] == "OK"
    return d


def guardar(fig, nombre):
    os.makedirs(PLOTS_DIR, exist_ok=True)
    ruta_plot = os.path.join(PLOTS_DIR, nombre)
    fig.savefig(ruta_plot, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    if os.path.isdir(os.path.dirname(REPORT_FIGURES_DIR)):
        os.makedirs(REPORT_FIGURES_DIR, exist_ok=True)
        shutil.copy2(ruta_plot, os.path.join(REPORT_FIGURES_DIR, nombre))
    print("Guardado:", nombre)


def fmt_size(ax):
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_xlabel("Tamaño de la grilla $N \cdot M$")


def grafico_tiempo(d):
    """Tiempo vs tamaño: un panel por tipo de grilla (promedio sobre dominio y muestras)."""
    fig, axes = plt.subplots(1, 4, figsize=(13, 3.4), sharey=True)
    for ax, tipo in zip(axes, TIPOS):
        sub = d[d.tipo == tipo]
        for a in ALGOS:
            s = sub[sub.algoritmo == a].groupby("size")["tiempo_ms"].mean()
            s = s[s > 0]
            ax.plot(s.index, s.values, marker=MARCA[a], color=COLOR[a], label=ETIQUETA[a])
        fmt_size(ax)
        ax.set_title(f"Grilla {tipo}")
    axes[0].set_ylabel("Tiempo de ejecución [ms]")
    axes[0].legend(loc="upper left")
    fig.suptitle("Tiempo de ejecución vs tamaño", y=1.02)
    guardar(fig, "tiempo_vs_tamano.png")


def grafico_tiempo_total(d):
    """Tiempo vs tamaño promediando todo: curvas con referencias asintóticas para DP y Greedy."""
    fig, ax = plt.subplots(figsize=(5.4, 3.8))
    for a in ALGOS:
        s = d[d.algoritmo == a].groupby("size")["tiempo_ms"].mean()
        s = s[s > 0]
        ax.plot(s.index, s.values, marker=MARCA[a], color=COLOR[a], label=ETIQUETA[a])
    # Referencia lineal en N*M (pendiente 1) anclada en el DP de tamaño mayor
    sdp = d[d.algoritmo == "dp"].groupby("size")["tiempo_ms"].mean()
    sdp = sdp[sdp > 0]
    if len(sdp) >= 2:
        x_min = max(sdp.index.min() * 4, sdp.index[len(sdp) // 2])
        x = np.array([x_min, sdp.index.max()], dtype=float)
        ax.plot(x, sdp.iloc[-1] * x / x[-1], color="gray", ls="--", lw=1, label="$\propto N M$ (ref.)")
    fmt_size(ax)
    ax.set_ylabel("Tiempo de ejecución [ms]")
    ax.set_title("Tiempo vs tamaño (promedio de todas las instancias)")
    ax.legend(loc="upper left")
    guardar(fig, "tiempo_vs_tamano_global.png")


def grafico_memoria(d):
    fig, ax = plt.subplots(figsize=(5.4, 3.8))
    for a in ALGOS:
        s = d[d.algoritmo == a].groupby("size")["memoria_kb"].mean()
        s = s[s > 0]
        ax.plot(s.index, s.values, marker=MARCA[a], color=COLOR[a], label=ETIQUETA[a])
    sdp = d[d.algoritmo == "dp"].groupby("size")["memoria_kb"].mean()
    sdp = sdp[sdp > 0]
    if len(sdp) >= 2:
        x_min = max(sdp.index.min() * 4, sdp.index[len(sdp) // 2])
        x = np.array([x_min, sdp.index.max()], dtype=float)
        ax.plot(x, sdp.iloc[-1] * x / x[-1], color="gray", ls="--", lw=1, label="$\propto N M$ (ref.)")
    fmt_size(ax)
    ax.set_ylabel("Memoria de heap adicional (pico) [KiB]")
    ax.set_title("Uso de memoria vs tamaño")
    ax.legend(loc="upper left")
    guardar(fig, "memoria_vs_tamano.png")


def tabla_pareada(d):
    """Une DP (óptimo) y Greedy por instancia."""
    clave = ["n", "m", "size", "tipo", "dominio", "muestra"]
    dp = d[d.algoritmo == "dp"].set_index(clave)
    gr = d[d.algoritmo == "greedy"].set_index(clave)
    j = dp[["puntaje", "ok"]].join(gr[["puntaje", "ok"]], lsuffix="_opt", rsuffix="_gr").reset_index()
    j["falla"] = ~j["ok_gr"]
    j["delta"] = j["puntaje_opt"] - j["puntaje_gr"]
    return j


def grafico_brecha(j):
    """Brecha media Delta vs tamaño, panel por dominio, serie por tipo (solo éxitos de greedy)."""
    ok = j[~j.falla]
    fig, axes = plt.subplots(1, 2, figsize=(10, 3.6))
    for ax, dom in zip(axes, DOMINIOS):
        for t in TIPOS:
            s = ok[(ok.dominio == dom) & (ok.tipo == t)].groupby("size")["delta"].mean()
            s = s[s > 0]
            ax.plot(s.index, s.values, marker=MARCA_TIPO[t], color=COLOR_TIPO[t], label=t)
        fmt_size(ax)
        ax.set_title(f"Dominio {dom}")
    axes[0].set_ylabel("Brecha media $\Delta = s(C^*) - s(C_{greedy})$")
    axes[0].legend(loc="upper left", title="Tipo")
    fig.suptitle("Brecha de puntaje de Greedy respecto al óptimo (solo casos donde Greedy llega a la meta)", y=1.02)
    guardar(fig, "brecha_greedy.png")


def grafico_brecha_relativa(j):
    """Brecha normalizada por la escala del dominio (Delta / x), para comparar D10 vs D1000."""
    ok = j[~j.falla].copy()
    escala = ok["dominio"].str.replace("D", "", regex=False).astype(float)
    ok["delta_norm"] = ok["delta"] / escala
    fig, ax = plt.subplots(figsize=(5.4, 3.8))
    for dom, ls in zip(DOMINIOS, ["-", "--"]):
        s = ok[ok.dominio == dom].groupby("size")["delta_norm"].mean()
        s = s[s > 0]
        ax.plot(s.index, s.values, ls=ls, marker="o", color="#0072B2", label=dom)
    fmt_size(ax)
    ax.set_ylabel("$\Delta$ / (escala del dominio)")
    ax.set_title("Brecha normalizada por la escala del dominio")
    ax.legend(loc="upper left")
    guardar(fig, "brecha_normalizada.png")


def grafico_falla(j):
    """Tasa de falla de Greedy por tipo y dominio, y por tamaño."""
    fig, axes = plt.subplots(1, 2, figsize=(10, 3.6))
    ax = axes[0]
    w = 0.38
    x = np.arange(len(TIPOS))
    for k, (dom, col) in enumerate(zip(DOMINIOS, ["#0072B2", "#E69F00"])):
        vals = [100 * j[(j.tipo == t) & (j.dominio == dom)]["falla"].mean() for t in TIPOS]
        b = ax.bar(x + (k - 0.5) * w, vals, w, color=col, label=dom, edgecolor="white", linewidth=1)
        ax.bar_label(b, fmt="%.0f%%", fontsize=7, padding=2)
    ax.set_xticks(x, TIPOS)
    ax.set_ylabel("Tasa de falla de Greedy [%]")
    ax.set_title("Por tipo de grilla y dominio")
    ax.legend(title="Dominio", loc="upper left")
    ax.set_ylim(0, max(10, ax.get_ylim()[1] * 1.15))

    ax = axes[1]
    ns = sorted(j.n.unique())
    for t in ["murallas", "mixta", "minas"]:
        vals = [100 * j[(j.tipo == t) & (j.n == n)]["falla"].mean() for n in ns]
        ax.plot(range(len(ns)), vals, marker=MARCA_TIPO[t], color=COLOR_TIPO[t], label=t)
    ax.set_xticks(range(len(ns)), [str(n) for n in ns])
    ax.set_xlabel("Número de filas $N$")
    ax.set_ylabel("Tasa de falla de Greedy [%]")
    ax.set_title("Por tamaño (tipos con obstáculos)")
    ax.legend(loc="upper right")
    fig.suptitle("Tasa de falla de Greedy (SIN SOLUCION); el tipo 'libre' siempre es 0%", y=1.02)
    guardar(fig, "tasa_falla_greedy.png")


def tablas(d, j):
    """Tablas resumen para el informe."""
    t1 = d.groupby(["algoritmo", "size"]).agg(
        tiempo_ms=("tiempo_ms", "mean"), memoria_kb=("memoria_kb", "mean")).round(4).reset_index()
    t1.to_csv(os.path.join(MEASUREMENTS_DIR, "resumen_tiempo_memoria.csv"), index=False)

    t2 = j.groupby(["tipo", "dominio"]).agg(
        falla_pct=("falla", lambda s: 100 * s.mean()),
        delta_medio=("delta", lambda s: s[~j.loc[s.index, "falla"]].mean()),
        delta_max=("delta", lambda s: s[~j.loc[s.index, "falla"]].max()),
    ).round(2).reset_index()
    t2.to_csv(os.path.join(MEASUREMENTS_DIR, "resumen_calidad_greedy.csv"), index=False)

    t3 = j.groupby("n").agg(
        falla_pct=("falla", lambda s: 100 * s.mean()),
        delta_medio=("delta", lambda s: s[~j.loc[s.index, "falla"]].mean()),
    ).round(2).reset_index()
    t3.to_csv(os.path.join(MEASUREMENTS_DIR, "resumen_calidad_por_n.csv"), index=False)
    print("Tablas resumen escritas en", os.path.normpath(MEASUREMENTS_DIR))


def main():
    estilo()
    d = cargar()
    j = tabla_pareada(d)
    grafico_tiempo(d)
    grafico_tiempo_total(d)
    grafico_memoria(d)
    grafico_brecha(j)
    grafico_brecha_relativa(j)
    grafico_falla(j)
    tablas(d, j)


if __name__ == "__main__":
    main()