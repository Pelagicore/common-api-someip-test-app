
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( common-api-test-service
	mainService.cpp
)

TARGET_LINK_LIBRARIES( common-api-test-service
	${COMMONAPI_GENERATED_LIBRARIES}
)

INSTALL(TARGETS common-api-test-service RUNTIME DESTINATION bin)

configure_file(test-service.someip-service.in test-service.someip-service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/test-service.someip-service DESTINATION ${CMAKE_INSTALL_PREFIX}/share/someip-services)

# systemd files
if (ENABLE_SYSTEMD)
	message("install systemd files")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test-service.service.in  ${CMAKE_CURRENT_BINARY_DIR}/test-service.service)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/test-service.service DESTINATION ${SYSTEMD_CONFIGURATION_FILES_DIR})
endif()
