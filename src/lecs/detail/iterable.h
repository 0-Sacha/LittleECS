#pragma once

#include <iterator>

namespace lecs::detail
{
    template <typename Begin, typename End>
    struct Iterable
    {
        Iterable(Begin&& begin, End&& end)
            : begin_(std::forward<Begin>(begin))
            , end_(std::forward<End>(end))
        {}

        Begin begin()
        {
            return begin_;
        }

        End end()
        {
            return end_;
        }

    private:
        Begin begin_;
        End end_;
    };

    struct IterableEnd {};
}
