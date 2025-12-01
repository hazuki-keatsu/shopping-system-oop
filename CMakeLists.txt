cmake_minimum_required(VERSION 3.15)
project(ShoppingSystem VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译器选项
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/Include)

# 查找yaml-cpp库（如果需要的话，这里先简单实现不依赖外部库）
# find_package(yaml-cpp REQUIRED)

# 收集源文件
file(GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/Src/*.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 如果使用yaml-cpp
# target_link_libraries(${PROJECT_NAME} yaml-cpp)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# 复制配置文件和数据文件到输出目录
file(COPY ${PROJECT_SOURCE_DIR}/res
     DESTINATION ${PROJECT_SOURCE_DIR}/bin)
