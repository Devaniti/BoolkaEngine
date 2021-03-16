#include "pch.h"

// clang-format mess up formating due to preprocessor class definition
// clang-format off

namespace Boolka
{

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

        TEST_METHOD(Vector4HLSLStyleConstructor)
        {
            {
                const Vector3 v1{ 1.0f, 2.0f, 3.0f };
                const Vector4 v2{ v1, 4.0f };
                const Vector4 v3{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::IsTrue(v2 == v3);
            }

            {
                const Vector3 v1{ 1.0f, 2.0f, 3.0f };
                const Vector4 v2{ 0.0f, v1 };
                const Vector4 v3{ 0.0f, 1.0f, 2.0f, 3.0f };
                Assert::IsTrue(v2 == v3);
            }
        }
    };

    TEST_CLASS(TestVector4)
    {
    public:

        TEST_METHOD(Constructors)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const Vector4 v2(v1);
                Assert::IsTrue(v1 == v2);
            }

            {
                Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const Vector4 v2(v1);
                const Vector4 v3(std::move(v1));
                Assert::IsTrue(v2 == v3);
            }
        }

        TEST_METHOD(OperatorAssignment)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Vector4 v2;
                v2 = v1;
                Assert::IsTrue(v1 == v2);
            }

            {
                Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const Vector4 v2(v1);
                Vector4 v3;
                v3 = std::move(v1);
                Assert::IsTrue(v2 == v3);
            }
        }

        TEST_METHOD(HLSLStyleComponentAccess)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::IsTrue(v1.x() == 1.0f);
                Assert::IsTrue(v1.y() == 2.0f);
                Assert::IsTrue(v1.z() == 3.0f);
                Assert::IsTrue(v1.w() == 4.0f);

                Assert::IsTrue(v1.r() == 1.0f);
                Assert::IsTrue(v1.g() == 2.0f);
                Assert::IsTrue(v1.b() == 3.0f);
                Assert::IsTrue(v1.a() == 4.0f);
            }

            {
                const Vector4 v1;
                Assert::IsTrue(v1.x() == 0.0f);
                Assert::IsTrue(v1.y() == 0.0f);
                Assert::IsTrue(v1.z() == 0.0f);
                Assert::IsTrue(v1.w() == 0.0f);

                Assert::IsTrue(v1.r() == 0.0f);
                Assert::IsTrue(v1.g() == 0.0f);
                Assert::IsTrue(v1.b() == 0.0f);
                Assert::IsTrue(v1.a() == 0.0f);
            }

            {
                Vector4 v1;
                v1.x() = 1.0f;
                v1.y() = 2.0f;
                v1.z() = 3.0f;
                v1.w() = 4.0f;

                Assert::IsTrue(v1.x() == 1.0f);
                Assert::IsTrue(v1.y() == 2.0f);
                Assert::IsTrue(v1.z() == 3.0f);
                Assert::IsTrue(v1.w() == 4.0f);

                Assert::IsTrue(v1.r() == 1.0f);
                Assert::IsTrue(v1.g() == 2.0f);
                Assert::IsTrue(v1.b() == 3.0f);
                Assert::IsTrue(v1.a() == 4.0f);
            }

            {
                Vector4 v1;
                v1.r() = 5.0f;
                v1.g() = 6.0f;
                v1.b() = 7.0f;
                v1.a() = 8.0f;

                Assert::IsTrue(v1.x() == 5.0f);
                Assert::IsTrue(v1.y() == 6.0f);
                Assert::IsTrue(v1.z() == 7.0f);
                Assert::IsTrue(v1.w() == 8.0f);

                Assert::IsTrue(v1.r() == 5.0f);
                Assert::IsTrue(v1.g() == 6.0f);
                Assert::IsTrue(v1.b() == 7.0f);
                Assert::IsTrue(v1.a() == 8.0f);
            }
        }

        TEST_METHOD(OperatorSubscript)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::IsTrue(v1[0] == 1.0f);
                Assert::IsTrue(v1[1] == 2.0f);
                Assert::IsTrue(v1[2] == 3.0f);
                Assert::IsTrue(v1[3] == 4.0f);
            }

            {
                const Vector4 v1;
                Assert::IsTrue(v1[0] == 0.0f);
                Assert::IsTrue(v1[1] == 0.0f);
                Assert::IsTrue(v1[2] == 0.0f);
                Assert::IsTrue(v1[3] == 0.0f);
            }

            {
                Vector4 v1;
                v1[0] = 1.0f;
                v1[1] = 2.0f;
                v1[2] = 3.0f;
                v1[3] = 4.0f;
                Assert::IsTrue(v1[0] == 1.0f);
                Assert::IsTrue(v1[1] == 2.0f);
                Assert::IsTrue(v1[2] == 3.0f);
                Assert::IsTrue(v1[3] == 4.0f);
            }
        }

        TEST_METHOD(GetBuffer)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const float* buffer = v1.GetBuffer();
                Assert::IsTrue(buffer[0] == 1.0f);
                Assert::IsTrue(buffer[1] == 2.0f);
                Assert::IsTrue(buffer[2] == 3.0f);
                Assert::IsTrue(buffer[3] == 4.0f);
            }

            {
                const Vector4 v1;
                const float* buffer = v1.GetBuffer();
                Assert::IsTrue(buffer[0] == 0.0f);
                Assert::IsTrue(buffer[1] == 0.0f);
                Assert::IsTrue(buffer[2] == 0.0f);
                Assert::IsTrue(buffer[3] == 0.0f);
            }
        }

        TEST_METHOD(RangeBasedFor)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                float sum = 0.0f;
                int i = 0;
                for (float element : v1)
                {
                    sum += element;
                    Assert::IsTrue(v1[i] == element);
                    i++;
                }
                Assert::IsTrue(i == 4);

                Assert::AreEqual(sum, 10.0f, BLK_TEST_EPSILON);
            }

            {
                const Vector4 v1;
                float sum = 0.0f;
                int i = 0;
                for (float element : v1)
                {
                    sum += element;
                    Assert::IsTrue(v1[i] == element);
                    i++;
                }
                Assert::IsTrue(i == 4);

                Assert::AreEqual(sum, 0.0f, BLK_TEST_EPSILON);
            }
        }

        TEST_METHOD(Size)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::IsTrue(v1.size() == 4);
            }
        }

        TEST_METHOD(Dot)
        {
            const Vector4 zero;

            const Vector4 v1{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Vector4 v2{ 0.0f, 1.0f, 0.0f, 0.0f };
            const Vector4 v3{ 0.0f, 1.0f, 0.0f, 0.0f };

            Assert::AreEqual(v1.Dot(v2), 0.0f, BLK_TEST_EPSILON);
            Assert::AreEqual(v1.Dot(v1), 1.0f, BLK_TEST_EPSILON);
            Assert::AreEqual(v2.Dot(v3), 1.0f, BLK_TEST_EPSILON);

            const Vector4 v4{ 0.88255f, 0.42268f, 0.84975f, 0.92081f };
            const Vector4 v5{ 0.36429f, 0.31932f, 0.80389f, 0.81533f };

            Assert::AreEqual(v4.Dot(v5), 1.89034f, BLK_TEST_EPSILON);
        }

        TEST_METHOD(Cross)
        {
            {
                const Vector4 v1{ 0.9682531217f, 0.3881531017f, 0.6592234505f, 0.0f };
                const Vector4 v2{ 0.6086175780f, 0.3005454900f, 0.7235579039f, 0.0f };
                                  
                const Vector4 v3 = v1.Cross(v2);
                const Vector4 v4 = v2.Cross(v1);

                Assert::IsTrue(ApproxEqual(v3, { 0.0827246097f, -0.2993722193f, 0.0547673082f, 0.0f }));
                Assert::IsTrue(ApproxEqual(v4, { -0.0827246097f, 0.2993722193f, -0.0547673082f, 0.0f }));
                Assert::IsTrue(ApproxEqual(v3, -v4));
            }

            {
                const Vector4 v1{ 0.2439354559f, 0.1728947439f, 0.2894266721f, 0.0f };
                const Vector4 v2{ 0.2937444482f, 0.9406464867f, 0.6133925071f, 0.0f };

                const Vector4 v3 = v1.Cross(v2);
                const Vector4 v4 = v2.Cross(v1);

                Assert::IsTrue(ApproxEqual(v3, { -0.1661958418f, -0.0646107027f, 0.1786701584f, 0.0f }));
                Assert::IsTrue(ApproxEqual(v4, { 0.1661958418f, 0.0646107027f, -0.1786701584f, 0.0f }));
                Assert::IsTrue(ApproxEqual(v3, -v4));
            }
        }

        TEST_METHOD(Length)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::AreEqual(v1.LengthSqr(), 30.0f, BLK_TEST_EPSILON);
                Assert::AreEqual(v1.LengthSlow(), ::sqrt(30.0f), BLK_TEST_EPSILON);

                Assert::AreEqual(v1.Length3Sqr(), 14.0f, BLK_TEST_EPSILON);
                Assert::AreEqual(v1.Length3Slow(), ::sqrt(14.0f), BLK_TEST_EPSILON);
            }

            {
                const Vector4 v1{ 0.2619941986f, 0.7270292679f, 0.0284368138f, 0.6807101035f };
                Assert::AreEqual(v1.LengthSqr(), 1.061387414f, BLK_TEST_EPSILON);
                Assert::AreEqual(v1.LengthSlow(), 1.030236582f, BLK_TEST_EPSILON);

                Assert::AreEqual(v1.Length3Sqr(), 0.598021169f, BLK_TEST_EPSILON);
                Assert::AreEqual(v1.Length3Slow(), 0.773318284f, BLK_TEST_EPSILON);
            }
        }

        TEST_METHOD(Normalize)
        {
            // Lower accuracy due to rsqrt calculation
            static const float normalizeEpsilon = 2e-4f;

            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };

                Assert::IsTrue(ApproxEqual(v1.Normalize(), { 0.182574186f, 0.365148372f, 0.547722558f, 0.730296743f }, normalizeEpsilon));
                Assert::IsTrue(ApproxEqual(v1.Normalize3(), { 0.267261242f, 0.534522484f, 0.801783726f, 1.069044968f }, normalizeEpsilon));
            }

            {
                const Vector4 v1{ 0.2619941986f, 0.7270292679f, 0.0284368138f, 0.6807101035f };

                Assert::IsTrue(ApproxEqual(v1.Normalize(), { 0.254304888f, 0.705691567f, 0.027602217f, 0.660731832f }, normalizeEpsilon));
                Assert::IsTrue(ApproxEqual(v1.Normalize3(), { 0.338792195f, 0.940142348f, 0.036772458f, 0.880245712f }, normalizeEpsilon));
            }
        }


        TEST_METHOD(OperatorUnaryMinus)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Assert::IsTrue(ApproxEqual(-v1, { -1.0f, -2.0f, -3.0f, -4.0f }));
                Assert::IsTrue(ApproxEqual(v1, -(-v1)));
            }

            {
                const Vector4 v1{ 0.5303753734f, 0.9921091151f, 0.9410908429f, 0.7026015303f };
                Assert::IsTrue(ApproxEqual(-v1, { -0.5303753734f, -0.9921091151f, -0.9410908429f, -0.7026015303f }));
                Assert::IsTrue(ApproxEqual(v1, -(-v1)));
            }

            {
                const Vector4 v1;
                Assert::IsTrue(ApproxEqual(-v1, v1));
                Assert::IsTrue(ApproxEqual(v1, -(-v1)));
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

        TEST_METHOD(OpeartorMultiply)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 2.0f, 3.0f };
                Vector4 v2{ 0.0f, 1.0f, 2.0f, 3.0f };
                const Vector4 v3{ 0.0f, 0.0f, 4.0f, 9.0f };
                Assert::IsTrue(ApproxEqual(v2 * v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2 * v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 0.0f, -1.0f };
                Vector4 v2{ 1.0f, -1.0f, 0.0f, -1.0f };
                const Vector4 v3{ 0.0f, -2.0f, 0.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2 * v1, v3));
            }
        }

        TEST_METHOD(OpeartorMultiplyElement)
        {
            {
                Vector4 v1{ 0.0f, 1.0f, 2.0f, 3.0f };
                const Vector4 v2{ 0.0f, 2.0f, 4.0f, 6.0f };
                Assert::IsTrue(ApproxEqual(v1 * 2.0f, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 2.0f, -3.0f };
                const Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v1 * 0.0f, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 0.0f, -1.0f };
                const Vector4 v2{ 3.0f, -3.0f, 0.0f, 3.0f };
                Assert::IsTrue(ApproxEqual(v1 * -3.0f, v2));
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

        TEST_METHOD(OpeartorDivide)
        {
            {
                const Vector4 v1{ 1.0f, 1.0f, 1.0f, 1.0f };
                Vector4 v2{ 1.0f, 0.0f, -1.0f, -2.0f };
                const Vector4 v3{ 1.0f, 0.0f, -1.0f, -2.0f };
                Assert::IsTrue(ApproxEqual(v2 / v1, v3));
            }

            {
                const Vector4 v1{ 2.0f, 1.0f, 0.5f, -1.0f };
                Vector4 v2{ 5.0f, 3.0f, 1.0f, -1.0f };
                const Vector4 v3{ 2.5f, 3.0f, 2.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2 / v1, v3));
            }

            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2 / v1, v3));
            }
        }

        TEST_METHOD(OpeartorDivideElement)
        {
            {
                Vector4 v1{ 0.0f, 1.0f, 2.0f, 3.0f };
                const Vector4 v2{ 0.0f, 0.5f, 1.0f, 1.5f };
                Assert::IsTrue(ApproxEqual(v1 / 2.0f, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, -1.0f, 1.0f };
                const Vector4 v2{ -1.0f, 1.0f, -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v1 / 1.0f, v2));
            }

            {
                Vector4 v1{ -1.0f, 1.0f, 3.0f, -2.0f };
                const Vector4 v2{ 0.5f, -0.5f, -1.5f, 1.0f };
                Assert::IsTrue(ApproxEqual(v1 / -2.0f, v2));
            }
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

        TEST_METHOD(OpeartorPlus)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 1.0f, -1.0f };
                Vector4 v2{ 0.0f, 1.0f, 0.0f, 1.0f };
                const Vector4 v3{ 1.0f, 1.0f, 1.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2 + v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2 + v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 1.0f, 1.0f };
                Vector4 v2{ -2.0f, 0.0f, -2.0f, 0.0f };
                const Vector4 v3{ -2.0f, 2.0f, -1.0f, 1.0f };
                Assert::IsTrue(ApproxEqual(v2 + v1, v3));
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

        TEST_METHOD(OpeartorMinus)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, 0.0f, 1.0f };
                Vector4 v2{ 0.0f, 1.0f, 0.0f, 0.0f };
                const Vector4 v3{ -1.0f, 1.0f, 0.0f, -1.0f };
                Assert::IsTrue(ApproxEqual(v2 - v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 0.0f, 0.0f, 0.0f };
                Vector4 v2{ 0.0f, 0.0f, 0.0f, 0.0f };
                const Vector4 v3{ 0.0f, 0.0f, 0.0f, 0.0f };
                Assert::IsTrue(ApproxEqual(v2 - v1, v3));
            }

            {
                const Vector4 v1{ 0.0f, 2.0f, 0.0f, -2.0f };
                Vector4 v2{ -2.0f, 0.0f, 2.0f, 2.0f };
                const Vector4 v3{ -2.0f, -2.0f, 2.0f, 4.0f };
                Assert::IsTrue(ApproxEqual(v2 - v1, v3));
            }
        }

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

        TEST_METHOD(ComparisonMaskOperators)
        {
            {
                const Vector4 v1{ 1.0f, 0.0f, -1.0f, 0.0f };
                const Vector4 v2{ 0.0f, 0.0f,  0.0f, 0.0f };

                const Vector4 v3 = v1 > v2;
                Assert::IsTrue(v3[0] != 0.0f);
                Assert::IsTrue(v3[1] == 0.0f);
                Assert::IsTrue(v3[2] == 0.0f);
                Assert::IsTrue(v3[3] == 0.0f);

                const Vector4 v4 = v1 < v2;
                Assert::IsTrue(v4[0] == 0.0f);
                Assert::IsTrue(v4[1] == 0.0f);
                Assert::IsTrue(v4[2] != 0.0f);
                Assert::IsTrue(v4[3] == 0.0f);

                const Vector4 v5 = v1 >= v2;
                Assert::IsTrue(v5[0] != 0.0f);
                Assert::IsTrue(v5[1] != 0.0f);
                Assert::IsTrue(v5[2] == 0.0f);
                Assert::IsTrue(v5[3] != 0.0f);

                const Vector4 v6 = v1 <= v2;
                Assert::IsTrue(v6[0] == 0.0f);
                Assert::IsTrue(v6[1] != 0.0f);
                Assert::IsTrue(v6[2] != 0.0f);
                Assert::IsTrue(v6[3] != 0.0f);

                const Vector4 v7 = v1.EqualMask(v2);
                Assert::IsTrue(v7[0] == 0.0f);
                Assert::IsTrue(v7[1] != 0.0f);
                Assert::IsTrue(v7[2] == 0.0f);
                Assert::IsTrue(v7[3] != 0.0f);

                const Vector4 v8 = v1.NotEqualMask(v2);
                Assert::IsTrue(v8[0] != 0.0f);
                Assert::IsTrue(v8[1] == 0.0f);
                Assert::IsTrue(v8[2] != 0.0f);
                Assert::IsTrue(v8[3] == 0.0f);
            }
        }

        TEST_METHOD(Select)
        {
            {
                const Vector4 v1{ 1.0f, 2.0f, 3.0f, 4.0f };
                const Vector4 v2{ 4.0f, 3.0f, 2.0f, 1.0f };

                const Vector4 v3 = v1 > v2;

                const Vector4 v4 = v1.Select(v2, v3);
                Assert::IsTrue(v4 == Vector4{1.0f, 2.0f, 2.0f, 1.0f});
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
            Assert::AreEqual(v1.LengthSlow(), 1.0f, BLK_TEST_EPSILON);
            Assert::AreEqual(v1.LengthSqr(), 1.0f, BLK_TEST_EPSILON);

            const Vector2 v2{ 1.0f, 1.0f };
            Assert::AreEqual(v2.LengthSlow(), sqrt(2.0f), BLK_TEST_EPSILON);
            Assert::AreEqual(v2.LengthSqr(), 2.0f, BLK_TEST_EPSILON);
        }

        TEST_METHOD(Dot)
        {
            const Vector2 zero;

            const Vector2 v1{ 1.0f, 0.0f };
            const Vector2 v2{ 0.0f, 1.0f };
            const Vector2 v3{ 0.0f, 1.0f };

            Assert::AreEqual(v1.Dot(v2), 0.0f, BLK_TEST_EPSILON);
            Assert::AreEqual(v1.Dot(v1), 1.0f, BLK_TEST_EPSILON);
            Assert::AreEqual(v2.Dot(v3), 1.0f, BLK_TEST_EPSILON);

            const Vector2 v4{ 0.88255f, 0.42268f };
            const Vector2 v5{ 0.36429f, 0.31932f };

            Assert::AreEqual(v4.Dot(v5), 0.456474f, BLK_TEST_EPSILON);
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

}
