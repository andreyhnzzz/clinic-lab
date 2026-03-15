# Guía de Compilación — Clínica Management System

## Requisitos

- **CMake** ≥ 3.16
- **Qt 6.x** (Widgets, Charts, Concurrent)
- **Compilador C++17**: GCC 9+, Clang 9+, o MSVC 2019+

---

## Windows (Qt Creator — Recomendado)

### 1. Instalar Qt 6

1. Descargar desde [qt.io/download](https://www.qt.io/download).
2. Ejecutar el instalador y seleccionar:
   - Una versión de Qt 6.x (e.g., Qt 6.7.x)
   - **Qt Widgets** (incluido por defecto)
   - **Qt Charts**
   - **Qt Concurrent**
   - Compilador: **MSVC 2019/2022 64-bit** o **MinGW 64-bit**
3. Qt Creator se instala junto con Qt.

### 2. Abrir el Proyecto

1. Abrir **Qt Creator**.
2. **Archivo → Abrir archivo o proyecto...** (`Ctrl+O`).
3. Navegar a la carpeta `ClinicManager/` y seleccionar `CMakeLists.txt`.
4. Seleccionar el kit (e.g., `Qt 6.7.x MSVC2022 64-bit`).
5. Clic en **Configure Project**.

### 3. Compilar y Ejecutar

- **Compilar:** `Ctrl+B`
- **Ejecutar:** `Ctrl+R`

### Nota sobre estilos

Si la ventana se abre sin estilos (fondo blanco), ejecutar el binario desde la carpeta
`ClinicManager/` o copiar `resources/styles.qss` al directorio de build.

---

## Windows (Línea de Comandos)

```cmd
cd ClinicManager
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
.\ClinicManager.exe
```

### Despliegue Portable

```cmd
mkdir dist
copy Release\ClinicManager.exe dist\
windeployqt dist\ClinicManager.exe
```

La carpeta `dist/` resultante es portable.

---

## Linux

### Instalar Dependencias

**Ubuntu/Debian:**
```bash
sudo apt-get install qt6-base-dev libqt6charts6-dev qt6-base-dev-tools cmake build-essential
```

**Fedora:**
```bash
sudo dnf install qt6-qtbase-devel qt6-qtcharts-devel cmake gcc-c++
```

### Compilar

```bash
cd ClinicManager
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
./ClinicManager
```

---

## macOS

### Instalar Dependencias

```bash
brew install qt6 cmake
```

### Compilar

```bash
cd ClinicManager
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt6) -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
./ClinicManager
```

---

## Solución de Problemas

| Problema | Solución |
|----------|----------|
| `Could not find Qt6` | Verificar que Qt 6 esté en el PATH o configurar `CMAKE_PREFIX_PATH` |
| `Qt6Charts not found` | Instalar componente Qt Charts |
| `Qt6Concurrent not found` | Instalar componente Qt Concurrent |
| Sin estilos (fondo blanco) | Copiar `resources/styles.qss` al directorio de ejecución |
