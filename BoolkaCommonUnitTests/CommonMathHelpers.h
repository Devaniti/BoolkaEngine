#pragma once

#define TEST_EPSILON (1e-5f)

namespace Boolka
{
    inline bool ApproxEqual(float a, float b)
    {
        return abs(a - b) <= TEST_EPSILON;
    }

    template<size_t componentCount>
    bool ApproxEqual(const Vector<componentCount>& a, const Vector<componentCount>& b)
    {
        for (size_t i = 0; i < componentCount; i++)
        {
            if (!ApproxEqual(a[i], b[i]))
                return false;
        }
        return true;
    }

    inline bool ApproxEqual(const Matrix4x4 a, const Matrix4x4 b)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (!ApproxEqual(a[i], b[i]))
                return false;
        }
        return true;
    }

}