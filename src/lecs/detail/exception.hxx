#pragma once

#include <exception>

namespace lecs {
    class LECSException : public std::exception {};
    class LECSNotImpl : public LECSException {};
}  // namespace lecs
