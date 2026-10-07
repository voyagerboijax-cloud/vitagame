cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Define la variable VITASDK")
  endif()
endif()

project(demo_demonios C)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Demo Demonios")
set(VITA_TITLEID  "DEMO00001")
set(VITA_VERSION  "01.00")

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME}
  vita2d
  SceLibKernel_stub SceDisplay_stub SceGxm_stub SceSysmodule_stub
  SceCtrl_stub ScePgf_stub ScePvf_stub SceCommonDialog_stub
  freetype png jpeg z m c
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
)
