cmake_minimum_required(VERSION 2.6)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(emalloc)
enable_testing()
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -std=gnu11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -std=gnu++11")

#########
# Vous devez editer la ligne suivante pour y mettre vos logins
#########
set(VARIANTE_LOGINS bachetaa ribauts)
###

list(SORT VARIANTE_LOGINS)
if (VARIANTE_LOGINS MATCHES "login[123]")
  message(FATAL_ERROR "** ERREUR **: Vous devez modifier CMakeLists.txt pour y mettre vos logins")
endif()

#######
# Compilation standard avec googletest
#######

###
# sur Debian et al, les googletests sont à recompiler
# sur centOS6, c'est une bibliothèque avec son équivalent de pkg-config
# sinon, on utilise l'autodetection de Cmake
###
if(EXISTS /usr/src/gtest)
    add_subdirectory(/usr/src/gtest ./gtest)
else(EXISTS /usr/src/gtest)
  if(EXISTS /usr/bin/gtest-config)
    execute_process(COMMAND gtest-config --cppflags --cxxflags OUTPUT_VARIABLE CENTOS_GTEST_CFLAGS)
    execute_process(COMMAND gtest-config --ldflags --libs OUTPUT_VARIABLE CENTOS_GTEST_LDFLAGS)
    string(STRIP "${CENTOS_GTEST_CFLAGS}" CENTOS_GTEST_CFLAGS)
    string(STRIP "${CENTOS_GTEST_LDFLAGS}" CENTOS_GTEST_LDFLAGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CENTOS_GTEST_CFLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CENTOS_GTEST_LDFLAGS} -lgtest_main")
  else(EXISTS /usr/bin/gtest-config)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
  endif(EXISTS /usr/bin/gtest-config)
endif(EXISTS /usr/src/gtest)

##
# Si vous utilisé plusieurs fichiers, en plus de mem.c et les autres,
# pour votre allocateur il faut les ajouter ici
##
add_library(emalloc SHARED src/mem.c src/mem_internals.c src/mem_small.c src/mem_medium.c src/mem_large.c)

##
# Bibliothèque pour faire des tests en python
##

add_library(mempy SHARED tests/mempymodule.c)
target_link_libraries(mempy emalloc)

##
# Construction du programme de tests unitaires
##
add_executable(alloctest tests/alloctest.cc tests/test_mark.cc tests/test_generic.cc tests/test_buddy.cc tests/test_run_cpp.cc)
target_link_libraries(alloctest gtest gtest_main emalloc)
add_test(AllTestsAllocator alloctest)

##
# Ajout d'une cible pour lancer les tests de manière verbeuse
##
add_custom_target(check alloctest)

##
# Construction du shell
##
add_executable(memshell src/memshell.c)
target_link_libraries(memshell emalloc)

##
# Construction de l'archive
# inclu le .git si il est là
##
string(REPLACE ";" "_" LOGINS_SANS_POINTVIRGULE "${VARIANTE_LOGINS}")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH ${LOGINS_SANS_POINTVIRGULE})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "\\\\.o$"
  "^${PROJECT_SOURCE_DIR}/build/"
)
include(CPack)
