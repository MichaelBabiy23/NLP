cmake_minimum_required(VERSION 3.28)
project(NLP C)

set(CMAKE_C_STANDARD 11)

add_executable(NLP
        linked_list.c
        markov_chain.c
        tweets_generator.c)
