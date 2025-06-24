add_library(passwordmgr
    PasswordManager.cpp PasswordManager.h
)
target_include_directories(passwordmgr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(passwordmgr PUBLIC crypto)
