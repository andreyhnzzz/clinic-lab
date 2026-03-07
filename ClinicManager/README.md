# Clínica Management System

Sistema de gestión para clínica privada costarricense desarrollado con **C++17** y **Qt6**.

## Módulos

| # | Módulo | Descripción | Estructura de Datos |
|---|--------|-------------|---------------------|
| 1 | Cola de Atención | Gestión de pacientes por prioridad | Min-Heap (PriorityQueue<T>) |
| 2 | Búsqueda de Expedientes | Búsqueda multi-criterio | HashTable + Búsqueda Binaria |
| 3 | Árbol de Diagnósticos | Árbol N-ario ICD-10 | N-ary Tree |
| 4 | Laboratorio de Rendimiento | Benchmarking de algoritmos | Comparación empírica |

## Compilación

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
./ClinicManager
```

## Requisitos

- CMake ≥ 3.16
- Qt6 (Widgets, Charts, Concurrent)
- Compilador con soporte C++17

## Estructura

```
ClinicManager/
├── src/
│   ├── models/          # Paciente, Consulta, Diagnostico
│   ├── data_structures/ # PriorityQueue<T>, HashTable<K,V>, DiagnosisTree
│   ├── algorithms/      # Sorting (Bubble, Selection, Insertion, Quick) + Searching
│   ├── modules/         # Lógica de negocio por módulo
│   ├── utils/           # DataGenerator, PerformanceMeter, GraphPlotter
│   └── gui/             # Widgets Qt sin archivos .ui
└── resources/
    └── styles.qss       # Tema oscuro médico
```
