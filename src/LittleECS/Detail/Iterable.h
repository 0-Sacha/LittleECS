#pragma once

#include <iterator>

namespace LittleECS::Detail
{
    template <typename Begin, typename End>
	struct Iterable
	{
		Iterable(Begin&& begin, End&& end)
			: m_Begin(std::forward<Begin>(begin))
			, m_End(std::forward<End>(end))
		{}

		auto begin()
		{
            return m_Begin();
		}

		auto end()
		{
            return m_End();
		}

    private:
        Begin&& m_Begin;
        End&& m_End;
	};

	struct IterableEnd {};
}
