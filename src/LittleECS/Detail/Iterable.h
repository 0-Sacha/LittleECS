#pragma once

#include <iterator>

namespace LECS::Detail
{
    template <typename Begin, typename End>
	struct Iterable
	{
		Iterable(Begin&& begin, End&& end)
			: m_Begin(std::forward<Begin>(begin))
			, m_End(std::forward<End>(end))
		{}

		Begin begin()
		{
            return m_Begin;
		}

		End end()
		{
            return m_End;
		}

    private:
        Begin m_Begin;
        End m_End;
	};

	struct IterableEnd {};
}
