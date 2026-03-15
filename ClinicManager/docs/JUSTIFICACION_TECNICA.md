# Justificación Técnica — Clínica Management System

## 1. Cola de Atención — Min-Heap (PriorityQueue\<T\>)

**Estructura elegida:** Min-Heap implementado como array dinámico (QVector).

**Justificación:**
- Los pacientes críticos (prioridad 1) deben atenderse antes que urgentes (2) y normales (3).
- El Min-Heap garantiza `peek()` y `pop()` en **O(log n)**.
- Inserción también en **O(log n)**.
- Desempate por orden de llegada (campo `arrivalOrder`) para FIFO entre misma prioridad.

**Alternativas descartadas:**
- Lista enlazada ordenada: Inserción O(n).
- Array no ordenado: Búsqueda del mínimo O(n).
- Árbol AVL: Implementación más compleja con beneficio marginal.

## 2. Búsqueda de Expedientes

### 2a. HashTable para búsqueda por cédula

**Estructura:** HashTable propia con open addressing (sondeo lineal).

**Justificación:**
- La cédula es identificador único. Búsqueda O(1) promedio vs O(log n) de BST o O(n) lineal.
- Open addressing: Mejor localidad de caché que chaining.
- Factor de carga máximo 0.5: Resize al superar 50% para mantener O(1) amortizado.

### 2b. Búsqueda Binaria para rangos de fecha

**Justificación:**
- Consultas ordenadas por fecha permiten búsqueda de rango en O(log n + k).
- Búsqueda por nombre usa búsqueda lineal (nombres no son únicos ni ordenados).

## 3. Árbol de Diagnósticos — Árbol N-ario

**Estructura:** Árbol N-ario de 3 niveles: Área → Especialidad → Diagnóstico.

**Justificación:**
- La clasificación ICD-10 es jerárquica por naturaleza.
- El árbol N-ario modela esta jerarquía naturalmente.
- Recorridos implementados:
  - **Pre-Order:** Lista diagnósticos agrupados por área (padre antes que hijos).
  - **Post-Order:** Lista diagnósticos antes de sus categorías (hijos antes que padres).
  - **BFS:** Exploración nivel por nivel.
  - **Recorrido Completo:** Muestra TODOS los nodos (áreas, especialidades, diagnósticos) para demostración académica.
- Búsqueda O(n) aceptable dado el tamaño del árbol (~50 nodos).

## 4. Laboratorio de Rendimiento

### Algoritmos de Ordenamiento

| Algoritmo | Promedio | Peor Caso | Estable | Memoria |
|-----------|----------|-----------|---------|---------|
| Bubble Sort | O(n²) | O(n²) | Sí | O(1) |
| Selection Sort | O(n²) | O(n²) | No | O(1) |
| Insertion Sort | O(n²) | O(n²) | Sí | O(1) |
| Quick Sort | O(n log n) | O(n²) | No | O(log n) |
| std::sort | O(n log n) | O(n log n) | No | O(log n) |

**Decisiones:**
- Para n > 10,000, se omiten algoritmos O(n²) para evitar bloqueo de UI.
- Quick Sort usa median-of-three + cutoff a inserción para n < 16.
- std::sort (Introsort) se incluye como referencia pero sus swaps no son instrumentables sin reimplementación — esto queda documentado en la UI.

### Instrumentación de Métricas

Cada algoritmo instrumentado reporta:
- **Tiempo real** (chrono high_resolution_clock)
- **Comparaciones** (contador explícito)
- **Swaps/movimientos** (contador explícito)
- **Complejidad teórica**
- **Estabilidad**
- **Memoria auxiliar**
- **Notas metodológicas**

**Nota sobre std::sort:** Al ser una implementación de biblioteca estándar, no permite
instrumentar comparaciones ni swaps internos sin reimplementar el algoritmo. Los datos
de métricas para std::sort reflejan solo tiempo real y metadata teórica. Esto es transparente
en la UI.

### Datasets del Benchmark

Los benchmarks usan **datos reales del sistema** (pacientes y consultas del ClinicDataStore),
no arrays sintéticos de enteros. Para tamaños grandes que excedan los datos disponibles,
se generan registros adicionales con:
- Cédulas únicas
- Nombres y apellidos reales
- Cantones de Costa Rica
- Diagnósticos válidos
- Fechas y costos coherentes

La generación adicional no contamina el store principal.

### Comparación de Búsquedas

La comparación lineal vs binaria se ejecuta sobre:
- **Mismo dataset** (copia del mismo array)
- **Mismo campo** (seleccionado por el usuario)
- **Mismo target** (paciente/consulta existente en el medio del dataset)
- **Costo de ordenamiento separado** del costo puro de búsqueda binaria

## 5. Decisiones Tecnológicas

### C++17
- `std::optional<V>` para retornos seguros en HashTable.
- Structured bindings.
- Templates con lambdas como parámetros por defecto.

### Qt6
- `Qt6::Charts` para gráficas sin dependencias externas.
- `Qt6::Concurrent` para benchmarks asíncronos.
- Widgets programáticos (sin .ui) para portabilidad y control total.

### Tema Clinical Graphite
- Fondo oscuro (#1e1e1e): Reduce fatiga visual en uso prolongado.
- Acento cyan (#00BCD4): Asociado al sector salud.
- Badges de prioridad (rojo/naranja/verde): Estándares de triage hospitalario.

## 6. Datos Costarricenses
- **Cédulas:** Formato "X-XXXX-XXXX"
- **Cantones:** 15 cantones reales de Costa Rica
- **Nombres:** Combinación hispánica común en Costa Rica
- **Costos:** En colones costarricenses (₡15,000 – ₡515,000)
