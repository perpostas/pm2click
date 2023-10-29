cmake_minimum_required(VERSION 3.1)

# Authors:
# - Alexey Panov a.k.a Dyadya Lesha
# - Vladimir Kazansky a.k.a Dadya Vova 

project(pm2click)

find_package(LibXml2 REQUIRED)

include_directories(include ${LIBXML2_INCLUDE_DIR})

add_executable(pm2click src/pm2click.c)

target_link_libraries(pm2click ${LIBXML2_LIBRARIES})
