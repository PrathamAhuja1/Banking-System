add_library(core
    Bank.cpp Bank.h
    BankAccount.cpp BankAccount.h
    Transaction.cpp Transaction.h
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
