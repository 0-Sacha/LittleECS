#pragma once

#include <exception>

namespace LittleECS
{
    class LECSException : public std::exception {};
    class LECSNotImpl : public LECSException {};
}
