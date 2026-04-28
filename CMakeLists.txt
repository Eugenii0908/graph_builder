cmake_minimum_required(VERSION 3.16)

project(GraphBuilder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "D:/distr/QT/5.14.2/msvc2017_64")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Поиск Qt
find_package(Qt6 COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    set(QT_VERSION_MAJOR 5)
endif()

# Исходники
set(SOURCES
    src/main.cpp
    src/graph_builder.cpp
    src/graph_struct.cpp
)

# Заголовки
set(HEADERS
    include/graph_builder.h
    include/graph_struct.h
)

# Ресурсы (qrc)
set(RESOURCES
    resources/graph_builder.qrc
)

# Создаём исполняемый файл
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ui/graph_builder.ui
)

# Линкуем Qt
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Widgets
    )
else()
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
    )
endif()

# Добавляем пути к заголовкам
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_include_directories(${PROJECT_NAME} PRIVATE C:/Users/eubel/OneDrive/Desktop/graph/graph_builder/ui)

# Копирование Qt DLL в папку с exe
# Копирование Qt DLL и плагинов
if(WIN32)
    # Путь к Qt
    set(QT_BIN_DIR "D:/distr/QT/5.14.2/msvc2017_64/bin")
    set(QT_PLUGINS_DIR "D:/distr/QT/5.14.2/msvc2017_64/plugins")
    
    # Копирование DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_BIN_DIR}/Qt5Core.dll"
            "${QT_BIN_DIR}/Qt5Widgets.dll"
            "${QT_BIN_DIR}/Qt5Gui.dll"
            "${QT_BIN_DIR}/Qt5Cored.dll"
            "${QT_BIN_DIR}/Qt5Widgetsd.dll"
            "${QT_BIN_DIR}/Qt5Guid.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    # Копирование папки platforms
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QT_PLUGINS_DIR}/platforms"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
    )# Копирование стилей
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QT_PLUGINS_DIR}/styles"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles
    )
endif()