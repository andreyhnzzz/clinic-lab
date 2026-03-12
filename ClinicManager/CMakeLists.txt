cmake_minimum_required(VERSION 3.16)
project(ClinicManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Concurrent)

set(SOURCES
    main.cpp
    src/models/Paciente.cpp
    src/models/Consulta.cpp
    src/models/Diagnostico.cpp
    src/core/ClinicDataStore.cpp
    src/data_structures/DiagnosisTree.cpp
    src/modules/Module1_AttentionQueue.cpp
    src/modules/Module2_RecordsSearch.cpp
    src/modules/Module3_DiagnosisTree.cpp
    src/modules/Module4_PerformanceLab.cpp
    src/utils/DataGenerator.cpp
    src/utils/PerformanceMeter.cpp
    src/gui/MainWindow.cpp
    src/gui/AttentionQueueWidget.cpp
    src/gui/RecordsSearchWidget.cpp
    src/gui/DiagnosisTreeWidget.cpp
    src/gui/PerformanceLabWidget.cpp
)

set(HEADERS
    src/models/Paciente.h
    src/models/Consulta.h
    src/models/Diagnostico.h
    src/core/ClinicDataStore.h
    src/data_structures/PriorityQueue.h
    src/data_structures/DiagnosisTree.h
    src/data_structures/HashTable.h
    src/algorithms/sorting/BubbleSort.h
    src/algorithms/sorting/SelectionSort.h
    src/algorithms/sorting/InsertionSort.h
    src/algorithms/sorting/QuickSort.h
    src/algorithms/searching/LinearSearch.h
    src/algorithms/searching/BinarySearch.h
    src/modules/Module1_AttentionQueue.h
    src/modules/Module2_RecordsSearch.h
    src/modules/Module3_DiagnosisTree.h
    src/modules/Module4_PerformanceLab.h
    src/utils/DataGenerator.h
    src/utils/PerformanceMeter.h
    src/utils/GraphPlotter.h
    src/gui/MainWindow.h
    src/gui/AttentionQueueWidget.h
    src/gui/RecordsSearchWidget.h
    src/gui/DiagnosisTreeWidget.h
    src/gui/PerformanceLabWidget.h
)

qt_add_executable(ClinicManager
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
)

target_link_libraries(ClinicManager PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Concurrent
)

target_include_directories(ClinicManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
