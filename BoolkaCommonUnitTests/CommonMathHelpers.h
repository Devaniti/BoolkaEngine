#pragma once

#define BLK_TEST_EPSILON (1e-5f)

namespace Boolka
{
    inline bool ApproxEqual(float a, float b, float epsilon = BLK_TEST_EPSILON)
    {
        return abs(a - b) <= epsilon;
    }

    template <size_t componentCount>
    bool ApproxEqual(const Vector<componentCount>& a, const Vector<componentCount>& b,
                     float epsilon = BLK_TEST_EPSILON)
    {
        for (size_t i = 0; i < componentCount; i++)
        {
            if (!ApproxEqual(a[i], b[i], epsilon))
                return false;
        }
        return true;
    }

    inline bool ApproxEqual(const Matrix4x4 a, const Matrix4x4 b, float epsilon = BLK_TEST_EPSILON)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (!ApproxEqual(a[i], b[i], epsilon))
                return false;
        }
        return true;
    }

} // namespace Boolka