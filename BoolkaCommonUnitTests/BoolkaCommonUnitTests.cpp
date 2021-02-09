#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_EPSILON (1e-5f)

namespace Boolka
{
    template<size_t componentCount>
    bool ApproxEqual(const Vector<componentCount>& a, const Vector<componentCount>& b)
    {
        for (size_t i = 0; i < componentCount; i++)
        {
            if (abs(a[i] - b[i]) > TEST_EPSILON)
                return false;
        }
        return true;
    }

    bool ApproxEqual(const Matrix4x4 a, const Matrix4x4 b)
    {
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                if (abs(a[i][j] - b[i][j]) > TEST_EPSILON)
                    return false;
            }
        }
        return true;
    }

    TEST_CLASS(TestVectorConversion)
    {
    public:
        TEST_METHOD(Vector2ToVector4)
        {
            {
                const Vector2 v1{ 1.0f, 2.0f };
                const Vector4 v2 = v1;
                const Vector4 v3{ 1.0f, 2.0f, 0.0f, 0.0f };
                Assert::IsTrue(v2 == v3);
            }

            {
                const Vector2 v1{ 0.0f, 0.0f };
                const Vector4 v2 = v1;
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(v2 == v3);
            }

            {
                const Vector2 v1{ 0.0f, -1.0f };
                const Vector4 v2 = v1;
                const Vector4 v3{ 0.0f, -1.0f, 0.0f, 0.0f };
                Assert::IsTrue(v2 == v3);
            }
        }

        TEST_METHOD(Vector4ToVector2)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const Vector2 v2 = v1;
                const Vector2 v3{ 1.0f, 2.0f };
                Assert::IsTrue(v2 == v3);
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 3.0f, 4.0f };
                const Vector2 v2 = v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(v2 == v3);
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                const Vector2 v2 = v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(v2 == v3);
            }
        }
    };

    TEST_CLASS(TestVector2)
    {
    public:
        TEST_METHOD(OperatorEqual)
        {
            {
                const Vector2 v1;
                const Vector2 v2{ 0.0f, 0.0f };
                Assert::IsTrue(v1 == v2);
            }

            {
                const Vector2 v1{ 1.0f, 1.0f };
                const Vector2 v2{ 1.0f, 1.0f };
                Assert::IsTrue(v1 == v2);
            }

            {
                const Vector2 v1{ 1.0f, 1.0f };
                const Vector2 v2{ 0.0f, 1.0f };
                Assert::IsFalse(v1 == v2);
            }
        }

        TEST_METHOD(OperatorNotEqual)
        {
            {
                const Vector2 v1;
                const Vector2 v2{ 1.0f, 0.0f };
                Assert::IsTrue(v1 != v2);
            }

            {
                const Vector2 v1{ 0.0f, 1.0f };
                const Vector2 v2{ 1.0f, 0.0f };
                Assert::IsTrue(v1 != v2);
            }

            {
                const Vector2 v1{ 0.0f, 1.0f };
                const Vector2 v2{ 0.0f, 1.0f };
                Assert::IsFalse(v1 != v2);
            }
        }

        TEST_METHOD(Length)
        {
            const Vector2 v1{ 1.0f, 0.0f };
            Assert::AreEqual(v1.LengthSlow(), 1.0f, TEST_EPSILON);
            Assert::AreEqual(v1.LengthSqr(), 1.0f, TEST_EPSILON);

            const Vector2 v2{ 1.0f, 1.0f };
            Assert::AreEqual(v2.LengthSlow(), sqrt(2.0f), TEST_EPSILON);
            Assert::AreEqual(v2.LengthSqr(), 2.0f, TEST_EPSILON);
        }

        TEST_METHOD(Dot)
        {
            const Vector2 zero;

            const Vector2 v1{ 1.0f, 0.0f };
            const Vector2 v2{ 0.0f, 1.0f };
            const Vector2 v3{ 0.0f, 1.0f };

            Assert::AreEqual(v1.Dot(v2), 0.0f, TEST_EPSILON);
            Assert::AreEqual(v1.Dot(v1), 1.0f, TEST_EPSILON);
            Assert::AreEqual(v2.Dot(v3), 1.0f, TEST_EPSILON);

            const Vector2 v4{ 0.88255f, 0.42268f };
            const Vector2 v5{ 0.36429f, 0.31932f };

            Assert::AreEqual(v4.Dot(v5), 0.456474f, TEST_EPSILON);
        }

        TEST_METHOD(OpeartorPlusEqual)
        {
            {
                const Vector2 v1{ 1.0f, 0.0f };
                Vector2 v2{ 0.0f, 1.0f };
                v2 += v1;
                const Vector2 v3{ 1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 0.0f };
                Vector2 v2{ 0.0f, 0.0f };
                v2 += v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 2.0f };
                Vector2 v2{ -2.0f, 0.0f };
                v2 += v1;
                const Vector2 v3{ -2.0f, 2.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMinusEqual)
        {
            {
                const Vector2 v1{ 1.0f, 0.0f };
                Vector2 v2{ 0.0f, 1.0f };
                v2 -= v1;
                const Vector2 v3{ -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 0.0f };
                Vector2 v2{ 0.0f, 0.0f };
                v2 -= v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 2.0f };
                Vector2 v2{ -2.0f, 0.0f };
                v2 -= v1;
                const Vector2 v3{ -2.0f, -2.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMultiplyEqual)
        {
            {
                const Vector2 v1{ 1.0f, 0.0f };
                Vector2 v2{ 0.0f, 1.0f };
                v2 *= v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 0.0f };
                Vector2 v2{ 0.0f, 0.0f };
                v2 *= v1;
                const Vector2 v3{ 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 0.0f, 2.0f };
                Vector2 v2{ 1.0f, -1.0f };
                v2 *= v1;
                const Vector2 v3{ 0.0f, -2.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMultiplyEqualElement)
        {
            {
                Vector2 v1{ 0.0f, 1.0f };
                v1 *= 2.0f;
                const Vector2 v2{ 0.0f, 2.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector2 v1{ -1.0f, 1.0f };
                v1 *= 0.0f;
                const Vector2 v2{ 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector2 v1{ -1.0f, 1.0f };
                v1 *= -3.0f;
                const Vector2 v2{ 3.0f, -3.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }
        }

        TEST_METHOD(OpeartorDivideEqual)
        {
            {
                const Vector2 v1{ 1.0f, 1.0f };
                Vector2 v2{ 1.0f, 0.0f };
                v2 /= v1;
                const Vector2 v3{ 1.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 2.0f, 1.0f };
                Vector2 v2{ 5.0f, 3.0f };
                v2 /= v1;
                const Vector2 v3{ 2.5f, 3.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector2 v1{ 1.0f, 2.0f };
                Vector2 v2{ 0.0f, -1.0f };
                v2 /= v1;
                const Vector2 v3{ 0.0f, -0.5f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorDivideEqualElement)
        {
            {
                Vector2 v1{ 0.0f, 1.0f };
                v1 /= 2.0f;
                const Vector2 v2{ 0.0f, 0.5f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector2 v1{ -1.0f, 1.0f };
                v1 /= 1.0f;
                const Vector2 v2{ -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector2 v1{ -1.0f, 1.0f };
                v1 /= -2.0f;
                const Vector2 v2{ 0.5f, -0.5f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }
        }
    };

    TEST_CLASS(TestVector4)
    {
    public:
        TEST_METHOD(OperatorEqual)
        {
            {
                const Vector4 v1;
                const Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(v1 == v2);
            }

            {
                const Vector4 v1{ 1.0f, 0.0f, 1.0f, 0.0f };
                const Vector4 v2{ 1.0f, 0.0f, 1.0f, 0.0f };
                Assert::IsTrue(v1 == v2);
            }

            {
                const Vector4 v1{ 1.0f, 1.0f, 1.0f, 0.0f };
                const Vector4 v2{ 1.0f, 0.0f, 1.0f, 0.0f };
                Assert::IsFalse(v1 == v2);
            }
        }

        TEST_METHOD(OperatorNotEqual)
        {
            {
                const Vector4 v1;
                const Vector4 v2{ 1.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(v1 != v2);
            }

            {
                const Vector4 v1{ 0.0f, 1.0f, 0.0f, 1.0f };
                const Vector4 v2{ 1.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(v1 != v2);
            }

            {
                const Vector4 v1{ 1.0f, 0.0f, 1.0f, 0.0f };
                const Vector4 v2{ 1.0f, 0.0f, 1.0f, 0.0f };
                Assert::IsFalse(v1 != v2);
            }
        }

        TEST_METHOD(Length)
        {
            const Vector4 v1{ 1.0f, 0.0f, 0.0f, 0.0f };
            Assert::AreEqual(v1.LengthSlow(), 1.0f, TEST_EPSILON);
            Assert::AreEqual(v1.LengthSqr(), 1.0f, TEST_EPSILON);

            const Vector4 v2{ 1.0f, 1.0f, 0.0f, 0.0f };
            Assert::AreEqual(v2.LengthSlow(), sqrt(2.0f), TEST_EPSILON);
            Assert::AreEqual(v2.LengthSqr(), 2.0f, TEST_EPSILON);
        }

        TEST_METHOD(Dot)
        {
            const Vector4 zero;

            const Vector4 v1{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Vector4 v2{ 0.0f, 1.0f, 0.0f, 0.0f };
            const Vector4 v3{ 0.0f, 1.0f, 0.0f, 0.0f };

            Assert::AreEqual(v1.Dot(v2), 0.0f, TEST_EPSILON);
            Assert::AreEqual(v1.Dot(v1), 1.0f, TEST_EPSILON);
            Assert::AreEqual(v2.Dot(v3), 1.0f, TEST_EPSILON);

            const Vector4 v4{ 0.88255f, 0.42268f, 0.84975f, 0.92081f };
            const Vector4 v5{ 0.36429f, 0.31932f, 0.80389f, 0.81533f };

            Assert::AreEqual(v4.Dot(v5), 1.89034f, TEST_EPSILON);
        }

        TEST_METHOD(OpeartorPlusEqual)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 1.0f, -1.0f };
                Vector4 v2{ 0.0f, 1.0f, 0.0f, 1.0f };
                v2 += v1;
                const Vector4 v3{ 1.0f, 1.0f, 1.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                v2 += v1;
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 1.0f, 1.0f };
                Vector4 v2{ -2.0f, 0.0f, -2.0f, 0.0f };
                v2 += v1;
                const Vector4 v3{ -2.0f, 2.0f, -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMinusEqual)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 0.0f, 1.0f };
                Vector4 v2{ 0.0f, 1.0f, 0.0f, 0.0f };
                v2 -= v1;
                const Vector4 v3{ -1.0f, 1.0f, 0.0f, -1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                v2 -= v1;
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 0.0f, -2.0f };
                Vector4 v2{ -2.0f, 0.0f, 2.0f, 2.0f };
                v2 -= v1;
                const Vector4 v3{ -2.0f, -2.0f, 2.0f, 4.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMultiplyEqual)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 2.0f, 3.0f };
                Vector4 v2{ 0.0f, 1.0f, 2.0f, 3.0f };
                v2 *= v1;
                const Vector4 v3{ 0.0f, 0.0f, 4.0f, 9.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                v2 *= v1;
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 0.0f, -1.0f };
                Vector4 v2{ 1.0f, -1.0f, 0.0f, -1.0f };
                v2 *= v1;
                const Vector4 v3{ 0.0f, -2.0f, 0.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorMultiplyEqualElement)
        {
            {
                Vector4 v1{ 0.0f, 1.0f, 2.0f, 3.0f };
                v1 *= 2.0f;
                const Vector4 v2{ 0.0f, 2.0f, 4.0f, 6.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 2.0f, -3.0f };
                v1 *= 0.0f;
                const Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 0.0f, -1.0f };
                v1 *= -3.0f;
                const Vector4 v2{ 3.0f, -3.0f, 0.0f, 3.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }
        }

        TEST_METHOD(OpeartorDivideEqual)
        {
            {
                const Vector4 v1{ 1.0f, 1.0f, 1.0f, 1.0f };
                Vector4 v2{ 1.0f, 0.0f, -1.0f, -2.0f };
                v2 /= v1;
                const Vector4 v3{ 1.0f, 0.0f, -1.0f, -2.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 2.0f, 1.0f, 0.5f, -1.0f };
                Vector4 v2{ 5.0f, 3.0f, 1.0f, -1.0f };
                v2 /= v1;
                const Vector4 v3{ 2.5f, 3.0f, 2.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }

            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                v2 /= v1;
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2, v3));
            }
        }

        TEST_METHOD(OpeartorDivideEqualElement)
        {
            {
                Vector4 v1{ 0.0f, 1.0f, 2.0f, 3.0f };
                v1 /= 2.0f;
                const Vector4 v2{ 0.0f, 0.5f, 1.0f, 1.5f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, -1.0f, 1.0f };
                v1 /= 1.0f;
                const Vector4 v2{ -1.0f, 1.0f, -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 3.0f, -2.0f };
                v1 /= -2.0f;
                const Vector4 v2{ 0.5f, -0.5f, -1.5f, 1.0f };
                Assert::IsTrue(ApproxEqual(v1, v2));
            }
        }
    };

    TEST_CLASS(TestMatrix)
    {
    public:
        TEST_METHOD(OperatorEqual)
        {

            const Matrix4x4 m1;
            const Matrix4x4 m2
            {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
            };
            Assert::IsTrue(m1 == m2);

            const Matrix4x4 m3
            {
                0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
            };
            const Matrix4x4 m4
            {
                0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
            };

            Assert::IsTrue(m3 == m4);
            Assert::IsFalse(m1 == m3);
            Assert::IsFalse(m1 == m4);
            Assert::IsFalse(m2 == m3);
            Assert::IsFalse(m2 == m4);
        }

        TEST_METHOD(OperatorNotEqual)
        {
            const Matrix4x4 m1;
            const Matrix4x4 m2
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
            };
            Assert::IsTrue(m1 != m2);

            const Matrix4x4 m3
            {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
            };
            const Matrix4x4 m4
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
            };
            Assert::IsTrue(m3 != m4);
            Assert::IsTrue(m1 != m4);
            Assert::IsTrue(m2 != m3);

            Assert::IsFalse(m1 != m3);
            Assert::IsFalse(m2 != m4);
        }

        TEST_METHOD(OperatorMultiply)
        {
            {
                const Matrix4x4 m1 = Matrix4x4::GetTranslation(0.0f, 2.0f, 0.0f);
                const Matrix4x4 m2 = Matrix4x4::GetTranslation(2.0f, 0.0f, 0.0f);

                const Matrix4x4 m3 = Matrix4x4::GetTranslation(2.0f, 2.0f, 0.0f);

                Assert::IsTrue(ApproxEqual(m1 * m2, m3));
            }

            {
                const Matrix4x4 m1
                {
                    21, 22, 23, 24,
                    25, 26, 27, 28,
                    29, 30, 31, 32,
                    33, 34, 35, 36,
                };

                const Matrix4x4 m2
                {
                     5,  6,  7,  8,
                     9, 10, 11, 12,
                    13, 14, 15, 16,
                    17, 18, 19, 20,
                };

                const Matrix4x4 m3
                {
                    1010, 1100, 1190, 1280,
                    1186, 1292, 1398, 1504,
                    1362, 1484, 1606, 1728,
                    1538, 1676, 1814, 1952,
                };

                Assert::IsTrue(ApproxEqual(m1 * m2, m3));
            }

            {
                const Matrix4x4 m1 = Matrix4x4::GetTranslation(1.0f, 1.0f, 1.0f);
                const Matrix4x4 m2 = Matrix4x4::GetScale(0.5f);

                Assert::IsFalse(ApproxEqual(m1 * m2, m2 * m1));
            }
        }

        TEST_METHOD(OperatorMultiplyVector)
        {
            Vector4 v1{ 1, 2, 3, 4 };
            Matrix4x4 m1
            {
                 5,  6,  7,  8,
                 9, 10, 11, 12,
                13, 14, 15, 16,
                17, 18, 19, 20,
            };

            Vector4 v2{ 130, 140, 150, 160 };

            Assert::IsTrue(ApproxEqual(v1 * m1, v2));
        }

        TEST_METHOD(Transpose)
        {
            const Matrix4x4 m1{};

            Assert::IsTrue(ApproxEqual(m1, m1.Transpose()));

            const Matrix4x4 m2 = Matrix4x4::GetIdentity();

            Assert::IsTrue(ApproxEqual(m2, m2.Transpose()));
            Assert::IsFalse(ApproxEqual(m1, m2.Transpose()));

            const Matrix4x4 m3
            {
                 1,  2,  3,  4,
                 5,  6,  7,  8,
                 9, 10, 11, 12,
                13, 14, 15, 16,
            };

            Assert::IsTrue(ApproxEqual(m3, m3.Transpose().Transpose()));

            const Matrix4x4 m4
            {
                 1, 5,  9, 13,
                 2, 6, 10, 14,
                 3, 7, 11, 15,
                 4, 8, 12, 16,
            };

            Assert::IsTrue(ApproxEqual(m3, m4.Transpose()));
            Assert::IsTrue(ApproxEqual(m4, m3.Transpose()));
        }
    };
}
