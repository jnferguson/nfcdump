cmake_minimum_required(VERSION 2.8)
project(nfcdump)
add_definitions(-std=c++11 -Wall -ggdb)
include_directories(inc /usr/include/libusb-1.0)

add_executable(nfcdump
	src/main.cpp
	src/usb.cpp
	src/async.cpp
	src/hexdump.cpp
	src/cmd.cpp
	src/card.cpp
	src/pm3.cpp
	src/otfta.cpp
	src/parse.cpp
	src/file.cpp
	src/ncap.cpp
	src/miller.cpp
	src/manchester.cpp
	src/decoder.cpp
	src/sest.cpp
	inc/sest.hpp
	inc/decoder.hpp
	inc/manchester.hpp
	inc/miller.hpp
	inc/ncap.hpp
	inc/file.hpp
	inc/parse.hpp
	inc/otfta.hpp
	inc/usb.hpp
	inc/card.hpp
	inc/async.hpp
	inc/hexdump.hpp
	inc/cmd.hpp
	inc/pm3.hpp
)

target_link_libraries(nfcdump usb-1.0)
