# Arquitectura del Sistema — Clínica Management System

## Visión General

El sistema sigue una arquitectura modular de escritorio basada en **C++17** y **Qt6**,
con un repositorio central de datos (Singleton) y módulos de UI independientes
comunicados por señales y slots de Qt.

```
┌──────────────────────────────────────────────────┐
│                   MainWindow                      │
│  ┌──────────┐  ┌──────────────────────────────┐  │
│  │ Sidebar   │  │  QStackedWidget              │  │
│  │ (Nav)     │  │  ┌──────────────────────┐   │  │
│  │           │  │  │ DashboardWidget      │   │  │
│  │ Dashboard │  │  │ AttentionQueueWidget  │   │  │
│  │ Cola      │  │  │ RecordsSearchWidget   │   │  │
│  │ Expedient.│  │  │ DiagnosisTreeWidget   │   │  │
│  │ Diagnóst. │  │  │ PerformanceLabWidget  │   │  │
│  │ Lab Rend. │  │  │ AuditWidget           │   │  │
│  │ Auditoría │  │  │ EventLogWidget        │   │  │
│  │ Log       │  │  └──────────────────────┘   │  │
│  └──────────┘  └──────────────────────────────┘  │
└──────────────────────────────────────────────────┘
                        │
              ┌─────────┴─────────┐
              │  ClinicDataStore   │  (Singleton)
              │  - pacientes_      │
              │  - consultas_      │
              │  - cedulaIndex_    │
              │  - diagnosisTree_  │
              └───────────────────┘
                  │           │
    ┌─────────────┘           └──────────────┐
    │                                         │
┌───┴──────────┐                    ┌────────┴─────────┐
│ Estructuras  │                    │   Algoritmos      │
│ de Datos     │                    │                   │
│ - PriorityQ  │                    │ Sorting:          │
│ - HashTable  │                    │  BubbleSort       │
│ - DiagTree   │                    │  SelectionSort    │
└──────────────┘                    │  InsertionSort    │
                                    │  QuickSort        │
                                    │ Searching:        │
                                    │  LinearSearch     │
                                    │  BinarySearch     │
                                    └───────────────────┘
```

## Capas del Sistema

### 1. Capa de Modelos (`src/models/`)
- `Paciente.h` — Datos del paciente (cédula, nombre, edad, prioridad, cantón, diagnóstico, etc.)
- `Consulta.h` — Datos de la consulta médica (ID, cédula, fecha, diagnóstico estructurado, costo, gravedad)
- `Diagnostico.h` — Datos del diagnóstico ICD-10 (código, nombre, categoría, subcategoría)

### 2. Capa de Estructuras de Datos (`src/data_structures/`)
- `PriorityQueue<T>` — Min-Heap genérico con QVector. O(log n) inserción/extracción.
- `HashTable<K,V>` — Tabla hash con open addressing (sondeo lineal). O(1) promedio.
- `DiagnosisTree` — Árbol N-ario de 3 niveles: Área → Especialidad → Diagnóstico.

### 3. Capa de Algoritmos (`src/algorithms/`)
- **Ordenamiento**: Bubble Sort, Selection Sort, Insertion Sort, Quick Sort (median-of-three + cutoff a inserción).
- **Búsqueda**: Búsqueda Lineal (O(n)), Búsqueda Binaria (O(log n)).
- Todos implementados como templates header-only con instrumentación de métricas (SortMetrics).

### 4. Capa Core (`src/core/`)
- `ClinicDataStore` — Singleton. Repositorio central de pacientes, consultas e índices.
- `SystemAudit` — Auditoría automática: duplicados, huérfanos, prioridades, fechas, costos, índices.
- `EventLog` — Registro de eventos técnicos con niveles (info, warning, error, success).
- `JsonPersistence` — Persistencia JSON para pacientes y consultas.

### 5. Capa de Módulos de Negocio (`src/modules/`)
- `Module1_AttentionQueue` — Lógica de cola de atención con Min-Heap.
- `Module2_RecordsSearch` — Búsqueda por cédula (HashTable), nombre (lineal) y fecha (binaria).
- `Module3_DiagnosisTree` — Navegación y recorridos del árbol de diagnósticos.
- `Module4_PerformanceLab` — Benchmarking con datasets reales, métricas de sorting y searching.

### 6. Capa GUI (`src/gui/`)
- Widgets programáticos (sin archivos .ui) para máximo control.
- Tema **Clinical Graphite** (oscuro médico) aplicado via QSS.
- Comunicación por signals/slots de Qt.

## Patrones de Diseño

| Patrón | Uso |
|--------|-----|
| Singleton | `ClinicDataStore` — fuente única de verdad |
| Observer | Signals/slots de Qt para actualización reactiva |
| MVC (adaptado) | Modelos (structs) + Módulos (lógica) + Widgets (vista) |
| Template Method | Algoritmos genéricos de sorting/searching |
| Strategy | Comparadores lambda para ordenamiento por diferentes campos |

## Flujo Clínico Principal

```
Paciente llega → Se registra en Cola (Min-Heap)
    → Se atiende (dequeue del paciente más prioritario)
    → Se crea Consulta con diagnóstico estructurado (del árbol ICD-10)
    → Se actualiza ClinicDataStore
    → Dashboard, estadísticas y auditoría reflejan cambios automáticamente
```

## Concurrencia

- Los benchmarks del Laboratorio de Rendimiento se ejecutan en hilos secundarios
  usando `QtConcurrent::run()` para no bloquear la UI.
- El progreso se reporta al hilo principal mediante `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`.
