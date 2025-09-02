cmake_minimum_required(VERSION 3.15)

project(SPD_ProducerConsumer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Flags de Compilação (Debug e Release) ---

# Flags para o modo DEBUG
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")

# Flags para o modo RELEASE (otimizações e remoção de asserts)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# --- Estrutura do Projeto ---
include_directories(src)

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
