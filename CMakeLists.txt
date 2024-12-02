cmake_minimum_required(VERSION 3.20)
project(HealthCare)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        HealthCare
        src/main.cpp
        src/classes/Doctor/Doctor.cpp
        src/classes/Appointment/Appointment.cpp
)
