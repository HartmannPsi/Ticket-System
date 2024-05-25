#pragma once
#ifndef INPUT_PROCESS_HPP
#define INPUT_PROCESS_HPP

#include "account.hpp"
// #include "strcheck.hpp"
#include "time.hpp"
#include "train.hpp"
#include <iostream>

std::string slice(const std::string &str = "");

void process(const std::string &input);

void to_int_array(const std::string &str, int size, int *array);

void to_str_array(const std::string &str, int size, std::string *array);

#endif