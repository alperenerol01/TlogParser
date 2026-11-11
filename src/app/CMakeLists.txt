find_package(Qt6 6.8 REQUIRED COMPONENTS Quick Positioning Concurrent)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(TlogParser
    main.cpp
    TimeFormat.h
    TimeFormat.cpp
    models/PacketListModel.h
    models/PacketListModel.cpp
    controllers/LogSession.h
    controllers/LogSession.cpp
    controllers/PlaybackController.h
    controllers/PlaybackController.cpp
)

target_include_directories(TlogParser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    models
    controllers
)

qt_add_qml_module(TlogParser
    URI TlogParser
    VERSION 1.0
    QML_FILES
        views/Main.qml
        views/pages/LoadPage.qml
)

target_link_libraries(TlogParser PRIVATE TlogCore Qt6::Quick Qt6::Positioning Qt6::Concurrent)
