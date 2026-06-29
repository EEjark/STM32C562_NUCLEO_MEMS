# file-format: 1.0.0
# App layer: MEMSStudio protocol + BSP adaptation (ACC+GYR only)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/app_mems_protocol.c
  ${CMAKE_CURRENT_LIST_DIR}/mems_control.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)
