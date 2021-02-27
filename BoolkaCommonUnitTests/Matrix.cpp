#include "pch.h"

namespace Boolka
{

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

        TEST_METHOD(Inverse)
        {
            {
                bool isSuccessfull;
                const Matrix4x4 m1{};
                const Matrix4x4 m2 = m1.Inverse(isSuccessfull);

                // No inversed matrix for zero matrix
                Assert::IsTrue(!isSuccessfull);
            }

            {
                bool isSuccessfull;
                const Matrix4x4 m1
                {
                    9, 5, 8, 4,
                    7, 1, 9, 4,
                    1, 0, 9, 2,
                    0, 0, 0, 0,
                };
                const Matrix4x4 m2 = m1.Inverse(isSuccessfull);

                // No inversed matrix for matrix with zero determinant
                Assert::IsTrue(!isSuccessfull);
            }

            {
                bool isSuccessfull;
                const Matrix4x4 m1
                {
                    9, 5, 8, 4,
                    7, 1, 9, 4,
                    1, 0, 9, 2,
                    2, 9, 0, 5,
                };
                const Matrix4x4 m2 = m1.Inverse(isSuccessfull);

                const Matrix4x4 mult = m1 * m2;

                Assert::IsTrue(isSuccessfull);
                Assert::IsTrue(ApproxEqual(mult, Matrix4x4::GetIdentity()));
            }

            {
                bool isSuccessfull;
                const Matrix4x4 m1
                {
                    0, 1, 1, 0,
                    3, 6, 7, 3,
                    2, 2, 6, 3,
                    8, 8, 8, 2,
                };
                const Matrix4x4 m2 = m1.Inverse(isSuccessfull);

                const Matrix4x4 mult = m1 * m2;

                Assert::IsTrue(isSuccessfull);
                Assert::IsTrue(ApproxEqual(mult, Matrix4x4::GetIdentity()));
            }
        }
    };
}
