#include <iterator>
#include <cstddef>
#include <tuple>

namespace LittleECS
{
    class Registry;
    
    template <typename... ViewComponentTypes>
    class BasicView;

    template <typename BasicViewLinked, typename... IteratorComponentTypes>
    class ViewIterator
    {
    public:
        using ValueType = std::tuple<IteratorComponentTypes...>;

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = ValueType;
        using pointer           = ValueType*;
        using reference         = ValueType&;

    public:
        ViewIterator(BasicViewLinked basicViewLinked) : m_BasicViewLinked(basicViewLinked) {}

    private:

    public:
        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        ViewIterator& operator++() { m_ptr++; return *this; }
        ViewIterator operator++(int) { ViewIterator res(m_BasicViewLinked); ++(*this); return res; }

        bool operator==(const ViewIterator& lhs, const ViewIterator& rhs)
        {
            return a.m_ptr == b.m_ptr;
        }

        bool operator!=(const ViewIterator& lhs, const ViewIterator& rhs)
        {
            return a.m_ptr != b.m_ptr;
        }

    private:
        BasicViewLinked* m_BasicViewLinked;
    };
}
