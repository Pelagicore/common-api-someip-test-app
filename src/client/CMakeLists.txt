
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( common-api-someip-test-client
	mainClient.cpp
)

TARGET_LINK_LIBRARIES( common-api-someip-test-client
	${COMMONAPI_GENERATED_LIBRARIES}
)

INSTALL(TARGETS common-api-someip-test-client RUNTIME DESTINATION bin)
