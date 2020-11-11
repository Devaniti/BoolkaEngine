#pragma once

namespace Boolka
{

    template <typename T, size_t size>
    class RingBuffer
    {
    public:
        RingBuffer();
        ~RingBuffer();

        T* begin() { return m_Data; };
        T* end() { return m_Data + size; };

        T& Get() { return *m_Current; };
        void Flip();

    private:
        T* m_Current;
        T m_Data[size];
    };

    template <typename T, size_t size>
    Boolka::RingBuffer<T, size>::RingBuffer()
        : m_Data{}
        , m_Current(m_Data)
    {
    }

    template <typename T, size_t size>
    Boolka::RingBuffer<T, size>::~RingBuffer()
    {
    }

    template <typename T, size_t size>
    void Boolka::RingBuffer<T, size>::Flip()
    {
        ++m_Current;
        if (m_Current == end())
            m_Current = begin();
    }

}
