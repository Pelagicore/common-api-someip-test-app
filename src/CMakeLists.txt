
#add_commonapi_service(COMMONAPI_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface.fidl test/TestInterface)
#add_commonapi_dbus_service(COMMONAPI_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface-dbus.fdepl ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface.fidl test/TestInterface)
add_commonapi_someip_service(COMMONAPI_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface-someip.fdepl ${CMAKE_CURRENT_SOURCE_DIR}/TestInterface.fidl test/TestInterface)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory (service)
add_subdirectory (client)
