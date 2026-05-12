cmake_minimum_required(VERSION 3.28)
project(soccer_game_commented CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(soccer_game_commented Game.cpp League.cpp main.cpp Match.cpp Player.cpp Playoff.cpp Team.cpp Trade.cpp Utils.cpp)