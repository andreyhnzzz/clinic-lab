# Justificación de Decisiones de Diseño

## Clínica Management System — Proyecto Universitario

---

## 1. Módulo 1 — Cola de Atención: Min-Heap (PriorityQueue<T>)

**Estructura elegida:** Min-Heap implementado como array dinámico (QVector).

**Justificación:**
- En una clínica privada, los pacientes críticos (prioridad 1) deben atenderse antes que los urgentes (2) y los de rutina (3). El Min-Heap garantiza que `peek()` y `pop()` siempre entreguen el paciente de mayor prioridad en **O(log n)**.
- La inserción de nuevos pacientes también es **O(log n)**, lo cual es eficiente incluso con cientos de pacientes en espera.
- Alternativas descartadas:
  - **Lista enlazada ordenada:** Inserción O(n), inaceptable para volúmenes altos.
  - **Array no ordenado:** Búsqueda del mínimo O(n).
  - **Árbol AVL:** Implementación más compleja con beneficios marginales para este caso de uso.

---

## 2. Módulo 2 — Búsqueda de Expedientes

### 2a. HashTable para búsqueda por cédula

**Estructura elegida:** HashTable propia con open addressing (sondeo lineal).

**Justificación:**
- La cédula costarricense es un identificador único. La búsqueda por cédula es la operación más frecuente en recepción (verificar si el paciente existe, recuperar su historial).
- HashTable ofrece **O(1) promedio** para búsqueda e inserción, frente a **O(log n)** de un árbol BST o **O(n)** de búsqueda lineal.
- Se eligió open addressing sobre chaining porque:
  - Mejor localidad de caché (todos los datos en un array contiguo).
  - Menor overhead de memoria (sin punteros a nodos).
- Factor de carga máximo: 0.5 (resize al superar el 50%) para mantener O(1) amortizado.

### 2b. Búsqueda Binaria para rangos de fecha

**Algoritmo elegido:** Búsqueda binaria sobre array ordenado de consultas.

**Justificación:**
- Las consultas se mantienen ordenadas por fecha al insertarlas. Esto permite búsqueda de rango en **O(log n + k)** donde k es el número de resultados.
- Alternativa descartada: Búsqueda lineal O(n) — viable pero innecesariamente lenta para 200,000 registros.
- La búsqueda por nombre sí usa búsqueda lineal porque los nombres no son únicos y no hay un orden natural obvio que justifique mantener un índice separado.

---

## 3. Módulo 3 — Árbol de Diagnósticos: Árbol N-ario

**Estructura elegida:** Árbol N-ario de 3 niveles (Área → Especialidad → Diagnóstico).

**Justificación:**
- La clasificación médica ICD-10 es jerárquica por naturaleza: un diagnóstico pertenece a una especialidad, que pertenece a un área médica.
- El árbol N-ario modela esta jerarquía naturalmente, a diferencia de un árbol binario que forzaría una representación artificial.
- Se implementaron recorridos Pre-Order y BFS:
  - **Pre-Order:** Útil para listar diagnósticos agrupados por área (padre antes que hijos).
  - **BFS:** Útil para explorar nivel por nivel, permitiendo ver todas las áreas antes de entrar en especialidades.
- La búsqueda en el árbol es O(n) en el peor caso, aceptable dado que el árbol tiene pocas decenas de nodos diagnósticos.

---

## 4. Módulo 4 — Laboratorio de Rendimiento

**Algoritmos de ordenamiento implementados:**

| Algoritmo | Complejidad Promedio | Complejidad Peor | Estable |
|-----------|---------------------|------------------|---------|
| Bubble Sort | O(n²) | O(n²) | Sí |
| Selection Sort | O(n²) | O(n²) | No |
| Insertion Sort | O(n²) | O(n²) | Sí |
| Quick Sort | O(n log n) | O(n²) | No |
| std::sort | O(n log n) | O(n log n) | No* |

**Decisión de diseño:** Para n > 10,000, se omiten Bubble, Selection e Insertion porque su ejecución puede demorar minutos, haciendo la UI inutilizable. Se muestra una advertencia al usuario.

**Ejecución asíncrona:** Los benchmarks corren en un hilo secundario (QtConcurrent) para no bloquear la interfaz gráfica. El progreso se reporta mediante signals/slots.

**Comparación de búsquedas:**
- Se promedian 1000 ejecuciones para reducir el ruido de medición.
- Búsqueda lineal: O(n) — crece linealmente.
- Búsqueda binaria: O(log n) — crece logarítmicamente, imperceptible para el usuario en datasets grandes.

---

## 5. Decisiones Generales

### Por qué C++17
- `std::optional<V>` en HashTable para retornos seguros sin punteros nulos.
- Structured bindings (`auto [a, b] = ...`) en PerformanceLabWidget.
- Templates con funciones lambda como parámetros por defecto.

### Por qué Qt6
- API moderna con soporte nativo para C++17.
- `Qt6::Charts` permite gráficas sin dependencias externas.
- `Qt6::Concurrent` simplifica el paralelismo con `QtConcurrent::run()`.
- Widgets programáticos (sin .ui) facilitan la portabilidad y el control total del layout.

### Tema oscuro médico
- Fondo oscuro (#1e1e1e) reduce la fatiga visual en entornos clínicos con uso prolongado.
- Acento cyan (#00BCD4) es asociado al sector salud y tecnología médica.
- Badges de prioridad (rojo, naranja, verde) siguen los estándares de triage hospitalario.

---

## 6. Datos Costarricenses

- **Cédulas:** Formato "X-XXXX-XXXX" (provincia-número-dígito verificador simulado).
- **Cantones:** 15 cantones reales de Costa Rica.
- **Nombres:** Combinación de nombres y apellidos hispánicos comunes en Costa Rica.
- **Diagnósticos:** Basados en las principales causas de consulta médica en Costa Rica según la CCSS.
- **Costos de consulta:** En colones costarricenses (₡15,000 – ₡515,000).
