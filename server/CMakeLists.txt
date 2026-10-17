cmake_minimum_required(VERSION 3.16)
project(DiscardChatServer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
add_executable(DiscardChatServer main.cpp manager.cpp manager.h totp.cpp totp.h
    ../core/chatserver.cpp ../core/chatserver.h ../core/serverprotocol.h)
target_include_directories(DiscardChatServer PRIVATE ../core .)
target_link_libraries(DiscardChatServer PRIVATE Qt6::Core Qt6::Network Qt6::Sql)

# Optional offline algorithm checks; the user runs these before VM deployment.
option(DISCARD_MFA_CHECKS "Build the RFC 6238 TOTP verification executable" OFF)
if(DISCARD_MFA_CHECKS)
    add_executable(DiscardTotpChecks tests/totp_vectors.cpp totp.cpp)
    target_include_directories(DiscardTotpChecks PRIVATE .)
    target_link_libraries(DiscardTotpChecks PRIVATE Qt6::Core)
endif()
