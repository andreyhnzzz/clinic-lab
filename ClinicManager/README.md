# Clínica Management System

Sistema de gestión para clínica privada costarricense desarrollado con **C++17** y **Qt6**.

## Módulos

| # | Módulo | Descripción | Estructura de Datos |
|---|--------|-------------|---------------------|
| 0 | Dashboard Ejecutivo | Resumen KPI del sistema completo | Lectura del ClinicDataStore |
| 1 | Cola de Atención | Gestión de pacientes por prioridad | Min-Heap (PriorityQueue<T>) |
| 2 | Búsqueda de Expedientes | Búsqueda multi-criterio | HashTable + Búsqueda Binaria |
| 3 | Árbol de Diagnósticos | Árbol N-ario ICD-10 | N-ary Tree |
| 4 | Laboratorio de Rendimiento | Benchmarking de algoritmos | Comparación empírica + SortMetrics |
| 5 | Auditoría de Integridad | Validación del sistema | Verificación de índices/datos |
| 6 | Registro de Eventos | Log técnico filtrable | QVector<LogEntry> circular |

### Características Adicionales

- **Flujo clínico completo**: Paciente → Cola → Atender → Registrar Consulta → Actualizar expediente automáticamente
- **Persistencia JSON**: Guardar/cargar pacientes y consultas desde archivo
- **Análisis automático de benchmarks**: Conclusiones sobre algoritmos más rápidos, viabilidad O(n²), comparación de búsquedas
- **Métricas avanzadas de ordenamiento**: Comparaciones, complejidad teórica, estabilidad, memoria auxiliar
- **Auditoría de integridad**: Detección de duplicados, consultas huérfanas, prioridades/fechas/costos inválidos

---

## ▶️ Abrir y ejecutar en Qt Creator

### Prerrequisitos

1. **Qt 6.x** instalado desde [qt.io/download](https://www.qt.io/download).  
   Durante la instalación asegúrate de marcar los siguientes componentes bajo la versión de Qt que elijas (ej. `Qt 6.7.x`):
   - `Qt Widgets`
   - `Qt Charts`
   - `Qt Concurrent`
   - El compilador de tu preferencia: **MSVC 2019/2022** (recomendado en Windows) o **MinGW 64-bit**

2. **CMake ≥ 3.16** — Qt Creator lo incluye automáticamente; si usas el instalador oficial de Qt no necesitas instalarlo por separado.

3. **Qt Creator 13 o superior** — se instala junto con Qt desde el instalador oficial.

---

### Paso a paso

#### 1. Clonar o descargar el repositorio

```bash
git clone https://github.com/andreyhnzzz/clinic-lab.git
```

O descargarlo como ZIP y extraerlo.

#### 2. Abrir el proyecto en Qt Creator

1. Abre **Qt Creator**.
2. Ve a **Archivo → Abrir archivo o proyecto…** (`Ctrl+O`).
3. Navega hasta la carpeta `ClinicManager/` dentro del repositorio y selecciona el archivo **`CMakeLists.txt`**.
4. Haz clic en **Abrir**.

#### 3. Configurar el Kit

Qt Creator mostrará el diálogo **"Configure Project"**:

1. Selecciona el kit que corresponda a tu instalación de Qt 6, por ejemplo:  
   `Qt 6.7.x MSVC2022 64-bit` o `Qt 6.7.x MinGW 64-bit`.
2. Deja el directorio de build por defecto (Qt Creator crea `build-ClinicManager-<kit>-Debug` automáticamente).
3. Haz clic en **"Configure Project"**.

> ⚠️ Si el kit no aparece en la lista, ve a  
> **Herramientas → Opciones → Kits** y verifica que Qt 6 esté detectado correctamente.

#### 4. Compilar

Presiona **`Ctrl + B`** o haz clic en el botón 🔨 **Build** en la barra lateral izquierda.

La primera compilación tarda ~1-2 minutos. El progreso aparece en el panel **Compile Output** en la parte inferior.

#### 5. Ejecutar

Presiona **`Ctrl + R`** o haz clic en el botón ▶️ **Run**.

La aplicación se abrirá con el tema oscuro médico y los 4 módulos disponibles en las pestañas.

---

### Solución de problemas comunes

| Problema | Causa probable | Solución |
|----------|---------------|----------|
| `Could not find Qt6` | Qt 6 no está en el PATH o el kit no lo detecta | Verifica en **Herramientas → Opciones → Qt Versions** que aparezca `qmake` de Qt 6 |
| `Qt6Charts not found` | El componente Qt Charts no fue instalado | Abre el **Qt Maintenance Tool** y añade `Qt Charts` a tu instalación |
| `Qt6Concurrent not found` | El componente Qt Concurrent no fue instalado | Abre el **Qt Maintenance Tool** y añade `Qt Concurrent` |
| Error de compilación con MSVC: `/std:c++17` | El compilador no está en modo C++17 | Ya está configurado en `CMakeLists.txt`; asegúrate de usar MSVC 2019 o superior |
| La ventana se abre sin estilos (fondo blanco) | `styles.qss` no se encuentra en el directorio de trabajo | Ejecuta el binario **desde la carpeta `ClinicManager/`** o copia `resources/styles.qss` al directorio de build |

---

## Compilación desde la línea de comandos

```bash
cd ClinicManager
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)   # Linux/macOS
cmake --build . -- /m        # Windows (MSVC)
./ClinicManager               # Linux/macOS
.\ClinicManager.exe           # Windows
```

## Despliegue en Windows (portable)

Para crear un ejecutable portable que funcione fuera de Qt Creator:

```cmd
cd ClinicManager
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

REM Copiar el ejecutable a una carpeta de distribución
mkdir dist
copy Release\ClinicManager.exe dist\

REM Usar windeployqt para copiar las DLLs de Qt necesarias
windeployqt dist\ClinicManager.exe
```

La carpeta `dist/` resultante es portable y ejecutable en cualquier máquina Windows sin Qt instalado.

## Requisitos

- CMake ≥ 3.16
- Qt 6.x (Widgets, Charts, Concurrent)
- Compilador con soporte C++17 (GCC 9+, Clang 9+, MSVC 2019+)

## Estructura

```
ClinicManager/
├── CMakeLists.txt
├── main.cpp
├── src/
│   ├── models/          # Paciente, Consulta, Diagnostico
│   ├── core/            # ClinicDataStore, SystemAudit, EventLog, JsonPersistence
│   ├── data_structures/ # PriorityQueue<T>, HashTable<K,V>, DiagnosisTree
│   ├── algorithms/      # Sorting (Bubble, Selection, Insertion, Quick) + Searching
│   ├── modules/         # Lógica de negocio por módulo
│   ├── utils/           # DataGenerator, PerformanceMeter, GraphPlotter
│   └── gui/             # Dashboard, Cola, Expedientes, Diagnósticos, Lab, Auditoría, Log
└── resources/
    └── styles.qss       # Tema Clinical Graphite
```
