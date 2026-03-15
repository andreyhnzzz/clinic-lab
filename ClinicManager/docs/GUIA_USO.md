# Guía de Uso — Clínica Management System

## Inicio

Al ejecutar la aplicación, se muestra el **Dashboard Ejecutivo** con los KPI principales del sistema.
La navegación se realiza a través del **sidebar** lateral izquierdo.

---

## Módulos

### 0. Dashboard Ejecutivo

Resumen general del sistema:
- **Total Pacientes**: Pacientes registrados en el sistema.
- **Total Consultas**: Consultas médicas registradas.
- **Pacientes en Cola**: Pacientes en espera de atención.
- **Diagnósticos Cargados**: Diagnósticos finales (hojas) del árbol ICD-10.
- **Estado Auditoría**: Resultado de la última auditoría de integridad.
- **Última Actualización**: Hora de la última actualización del dashboard.

Los KPI se actualizan automáticamente cuando cambian los datos.

---

### 1. Cola de Atención

Gestiona la prioridad de atención de pacientes.

**Acciones:**
- **Registrar Paciente**: Abre un formulario para registrar un nuevo paciente.
  Campos: cédula, nombre, edad, prioridad (1-Crítico, 2-Urgente, 3-Normal), cantón, diagnóstico.
- **Atender Siguiente**: Atiende al paciente de mayor prioridad.
  Se abre un diálogo para registrar la consulta con:
  - Motivo de consulta
  - Gravedad (1-5)
  - Costo (CRC)
  - **Diagnóstico estructurado** seleccionado desde el árbol ICD-10
  - Notas adicionales

**Visualización:**
- Tabla de pacientes en espera ordenados por prioridad real de atención.
- KPI por tipo de prioridad (Crítico, Urgente, Normal).
- Tiempo de espera promedio.
- Historial de atención reciente.

---

### 2. Búsqueda de Expedientes

Permite buscar pacientes y ver su historial de consultas.

**Criterios de búsqueda:**
- **Por cédula**: Búsqueda exacta O(1) usando HashTable.
- **Por nombre**: Búsqueda parcial (contiene) usando búsqueda lineal.
- **Por rango de fecha**: Búsqueda binaria sobre consultas ordenadas.

**Historial:** Al seleccionar un paciente, se muestra su historial completo de consultas
incluyendo diagnóstico estructurado (código, nombre, área, especialidad).

---

### 3. Árbol de Diagnósticos

Navega la jerarquía de diagnósticos ICD-10: Área → Especialidad → Diagnóstico.

**Funcionalidades:**
- **Búsqueda por nombre**: Filtra diagnósticos por texto.
- **Búsqueda por código**: Filtra por código ICD-10 (e.g., I21, K35).
- **Filtrar por especialidad**: Lista diagnósticos de una especialidad específica.
- **Pre-Order**: Recorrido en profundidad (padre antes que hijos).
- **BFS (Nivel)**: Recorrido por niveles (amplitud).
- **Post-Order**: Recorrido en profundidad (hijos antes que padres).
- **Recorrido Completo**: Muestra TODOS los nodos del árbol (áreas, especialidades, diagnósticos)
  con los tres tipos de recorrido para demostración académica.

**Detalle:** Al seleccionar un diagnóstico, se muestra código, área, especialidad y descripción.

---

### 4. Laboratorio de Rendimiento

Benchmarking empírico de algoritmos de ordenamiento y búsqueda.

**Configuración:**
- **Tamaño del dataset**: 100, 500, 1K, 5K, 10K, 50K, 100K pacientes.
- **Tipo de dataset**: Pacientes del sistema real.
- **Campo de ordenamiento**: Nombre, cédula, edad, prioridad, fecha.
- **Algoritmos**: Seleccionar cuáles ejecutar (Bubble, Selection, Insertion, Quick, std::sort).

**Ejecución:**
- **Ejecutar**: Benchmark individual con la configuración actual.
- **Serie Completa**: Ejecuta benchmarks para todos los tamaños (genera datos de crecimiento).
- **Comparar Búsquedas**: Compara Lineal vs Binaria sobre el mismo dataset y target.

**Métricas reportadas:**
- Tiempo real (ms)
- Comparaciones
- Swaps/movimientos
- Complejidad teórica
- Estabilidad
- Memoria auxiliar
- Notas metodológicas

**Gráficas:**
- Gráfica de barras: Comparación de algoritmos para un tamaño.
- Gráfica de crecimiento: Comportamiento por tamaño (O(n²) vs O(n log n)).

**Análisis automático:** Conclusiones sobre algoritmo más rápido, comportamiento por tamaño,
costo de ordenamiento vs beneficio de búsqueda binaria, inviabilidad de O(n²) en tamaños grandes.

---

### 5. Auditoría de Integridad

Validación automática del sistema.

**Verificaciones:**
- Pacientes duplicados (cédulas repetidas)
- Consultas huérfanas (referencia a paciente inexistente)
- Prioridades fuera de rango (debe ser 1-3)
- Fechas inválidas (formato yyyy-MM-dd)
- Costos negativos
- Gravedad fuera de rango (debe ser 1-5)
- Consistencia de índices internos

**Resultado:** Reporte con entradas clasificadas como Crítico, Advertencia o Info.

---

### 6. Registro de Eventos

Log técnico filtrable con todos los eventos del sistema.

**Tipos de eventos:**
- Paciente agregado
- Paciente atendido
- Consulta registrada
- Benchmark ejecutado
- Auditoría ejecutada
- Errores de validación
- Operaciones de persistencia

**Filtros:** Por tipo (Info, Warning, Error, Success), por texto.

---

## Persistencia

- **Guardar**: Guarda pacientes y consultas en archivo JSON.
- **Cargar**: Restaura datos desde archivo JSON.
- Ubicación por defecto: Directorio de datos de la aplicación del sistema operativo.

---

## Flujo Clínico Completo

```
1. Registrar paciente en Cola de Atención
2. Atender siguiente paciente (se extrae del Min-Heap)
3. Registrar consulta con diagnóstico estructurado del árbol ICD-10
4. El expediente se actualiza automáticamente en ClinicDataStore
5. Dashboard y estadísticas reflejan los cambios
6. Auditoría verifica la integridad de los datos
```
