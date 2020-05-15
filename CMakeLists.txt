cmake_minimum_required(VERSION 3.15)
project(acs_lab_3)
find_package(LibArchive REQUIRED)
find_package(TBB REQUIRED)

find_package(Boost COMPONENTS locale system REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n)
set(CMAKE_CXX_STANDARD 20)

add_executable(acs_lab_3 main.cpp)
target_link_libraries(acs_lab_3 TBB::tbb ${LibArchive_LIBRARIES}
    Threads::Threads ${Boost_LIBRARIES} Boost::locale Boost::system
        ICU::uc ICU::i18n)
