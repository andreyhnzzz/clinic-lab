cmake_minimum_required(VERSION 3.21)
project(ClinicManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Concurrent)

set(SOURCES
    src/main.cpp

    # Models
    src/core/models/Patient.cpp
    src/core/models/Consulta.cpp
    src/core/models/Diagnostico.cpp

    # Structures
    src/core/structures/PriorityQueue.cpp
    src/core/structures/DiagnosisTree.cpp
    src/core/structures/SearchAlgorithms.cpp

    # Generators
    src/core/generators/DataGenerator.cpp

    # Benchmarks
    src/core/benchmarks/SortBenchmark.cpp
    src/core/benchmarks/SearchBenchmark.cpp

    # GUI Windows
    src/gui/windows/MainWindow.cpp
    src/gui/windows/PerformanceLabWindow.cpp

    # GUI Widgets
    src/gui/widgets/PatientTableWidget.cpp
    src/gui/widgets/DiagnosisTreeWidget.cpp
    src/gui/widgets/ChartWidget.cpp

    # GUI Delegates
    src/gui/delegates/HighlightDelegate.cpp

    # Utils
    src/utils/TimerUtils.cpp
    src/utils/CSVExporter.cpp
    src/utils/CostaRicanData.cpp
)

set(HEADERS
    src/core/models/Patient.h
    src/core/models/Consulta.h
    src/core/models/Diagnostico.h
    src/core/structures/PriorityQueue.h
    src/core/structures/DiagnosisTree.h
    src/core/structures/SearchAlgorithms.h
    src/core/generators/DataGenerator.h
    src/core/benchmarks/SortBenchmark.h
    src/core/benchmarks/SearchBenchmark.h
    src/gui/windows/MainWindow.h
    src/gui/windows/PerformanceLabWindow.h
    src/gui/widgets/PatientTableWidget.h
    src/gui/widgets/DiagnosisTreeWidget.h
    src/gui/widgets/ChartWidget.h
    src/gui/delegates/HighlightDelegate.h
    src/utils/TimerUtils.h
    src/utils/CSVExporter.h
    src/utils/CostaRicanData.h
)

qt_add_executable(ClinicManager
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
)

target_include_directories(ClinicManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/structures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/generators
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/benchmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/windows
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/delegates
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

target_link_libraries(ClinicManager PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Concurrent
)

# Windows-specific: deploy Qt DLLs if windeployqt is available
if(WIN32)
    add_custom_command(TARGET ClinicManager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build complete. Run windeployqt on the executable if needed."
    )
endif()

# ── Tests ──────────────────────────────────────────────────────────────────
enable_testing()

qt_add_executable(test_structures
    tests/test_structures.cpp
    src/core/models/Patient.cpp
    src/core/models/Consulta.cpp
    src/core/models/Diagnostico.cpp
    src/core/structures/PriorityQueue.cpp
    src/core/structures/DiagnosisTree.cpp
    src/core/structures/SearchAlgorithms.cpp
    src/core/generators/DataGenerator.cpp
    src/utils/CostaRicanData.cpp
    src/utils/TimerUtils.cpp
)

target_include_directories(test_structures PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/structures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/generators
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

target_link_libraries(test_structures PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
)

add_test(NAME StructureTests COMMAND test_structures)
