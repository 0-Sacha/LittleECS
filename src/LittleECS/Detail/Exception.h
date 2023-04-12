#pragma once

#include <exception>

namespace LECS
{
    class LECSException : public std::exception {};
    class LECSNotImpl : public LECSException {};
}
