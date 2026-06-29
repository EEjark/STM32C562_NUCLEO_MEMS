# file-format: 1.0.0
# ComProtocol middleware: UART byte-stuffing + checksum + frame exchange.
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Src/com.c
  ${CMAKE_CURRENT_LIST_DIR}/Src/serial_protocol.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/Inc
)
