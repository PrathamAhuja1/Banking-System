set(GUI_SOURCES
    main.cpp
    LoginDialog.cpp LoginDialog.h
    MainWindow.cpp MainWindow.h
)

add_executable(SecureBankingApp ${GUI_SOURCES})
target_include_directories(SecureBankingApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SecureBankingApp
    Qt6::Widgets
    core
    crypto
    passwordmgr
    compression
)
