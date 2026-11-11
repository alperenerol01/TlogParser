find_package(Qt6 6.8 REQUIRED COMPONENTS Test Qml Positioning Concurrent)

set(CMAKE_AUTOMOC ON)

set(APP_DIR ${PROJECT_SOURCE_DIR}/src/app)

add_executable(TlogParserTest
    TlogParserTest.cpp
    ${APP_DIR}/TimeFormat.h
    ${APP_DIR}/TimeFormat.cpp
    ${APP_DIR}/models/PacketListModel.h
    ${APP_DIR}/models/PacketListModel.cpp
    ${APP_DIR}/controllers/LogSession.h
    ${APP_DIR}/controllers/LogSession.cpp
    ${APP_DIR}/controllers/PlaybackController.h
    ${APP_DIR}/controllers/PlaybackController.cpp
)

target_include_directories(TlogParserTest PRIVATE
    ${APP_DIR}
    ${APP_DIR}/models
    ${APP_DIR}/controllers
)

target_link_libraries(TlogParserTest PRIVATE
    TlogCore Qt6::Test Qt6::Qml Qt6::Positioning Qt6::Concurrent)

target_compile_definitions(TlogParserTest PRIVATE
    SAMPLE_TLOG_PATH="${PROJECT_SOURCE_DIR}/example-drone.tlog")

add_test(NAME TlogParserTest COMMAND TlogParserTest)

if(WIN32)
    set_tests_properties(TlogParserTest PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
endif()
