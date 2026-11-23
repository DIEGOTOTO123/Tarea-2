# AlgoReportTemplate

## Descripción

Este repositorio contiene una plantilla en **LaTeX** diseñada específicamente para la creación de mini-informes en la asignatura **Algoritmos y Complejidad** del Departamento de Informática de la **Universidad Técnica Federico Santa María (UTFSM)**. La plantilla está estructurada para facilitar la redacción de tareas, reportes de laboratorio y proyectos relacionados con el análisis de algoritmos.

### Modificación de la plantilla

- Escriba sus datos en `author.tex`.
- `preamble.tex` y `report.tex` NO debe ser modificados.
- `references.bib` contiene las referencias bibliográficas. Agregue las suyas.
- `report.pdf` es el archivo de salida generado por `pdflatex`.
- Elimine las figuras existentes y utilice las que generará en su informe.

```bash
.
├── author.tex
├── preamble.tex
├── README.md
├── references.bib
├── report.pdf
├── report.tex
└── sections
    ├── conclusions.tex
    ├── experiment_intro.tex
    ├── experiment_results.tex
    └── introduccion.tex

2 directories, 10 files

```

### Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

### Compilación del informe

Desde el directorio `report/` (tras haber generado los gráficos en `data/plots/`):

```bash
mkdir -p figures
cp ../code/grid_pathfinding/data/plots/*.png figures/
pdflatex -interaction=nonstopmode report.tex
biber report
pdflatex -interaction=nonstopmode report.tex
pdflatex -interaction=nonstopmode report.tex
```

### Dependencias y paquetes LaTeX

Para compilar `report.pdf` en Linux/WSL se requiere una distribución TeX completa o con los paquetes habituales de redacción científica y bibliografía:

* **Herramientas base:** `pdflatex` y `biber` (incluidos en distribuciones como **TeX Live**, por ejemplo TeX Live 2025 en Ubuntu/WSL).
* **Estructura, tipografía y formato:** `inputenc` / `fontenc` (o `babel` en español), `geometry`, `setspace`, `titlesec`, `fancyhdr` / `enumitem` / `parskip` (según defina `preamble.tex`).
* **Matemática y tablas:** `amsmath`, `amssymb`, `amsthm`, `booktabs`, `array`, `multirow`, `siunitx` (si aplica).
* **Gráficos y referencias:** `graphicx`, `float`, `subcaption` / `caption`, `hyperref`, `xcolor`, y `biblatex` con backend `biber` para `references.bib`.

```bash
sudo apt update
sudo apt install -y texlive-latex-base texlive-latex-extra texlive-science texlive-lang-spanish biber
```

En sistemas Debian/Ubuntu/WSL, si falta algún paquete al compilar, se puede instalar mediante los metapaquetes de TeX Live (por ejemplo `texlive-latex-extra`, `texlive-science`, `texlive-lang-spanish` y `biber`) y volver a ejecutar la compilación.