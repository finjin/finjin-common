//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/Angle.hpp"
#include "finjin/common/ValueOrError.hpp"
#include <Eigen/Eigen>


//Macros------------------------------------------------------------------------
#define FINJIN_PI 3.1415926535f
#define FINJIN_INTERNAL_MATRIX_STORAGE_ORDER Finjin::Common::MatrixStorageOrder::COLUMNS //Matrix types use Eigen column storage order


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Ordering of values within a 1D array representing a 2D matrix
    enum class MatrixStorageOrder
    {
        COLUMNS,
        ROWS
    };

    typedef Eigen::Matrix2f MathMatrix2;
    typedef Eigen::Matrix3f MathMatrix3;
    typedef Eigen::Matrix4f MathMatrix4;

    typedef Eigen::Vector2f MathVector2;
    typedef Eigen::Vector3f MathVector3;
    typedef Eigen::Vector4f MathVector4;

    typedef Eigen::Quaternionf MathQuaternion;

    typedef Eigen::Affine3f MathAffineTransform;

    typedef Eigen::Translation3f MathTranslation;
    typedef Eigen::AlignedScaling3f MathScaling;

    typedef Eigen::AngleAxisf MathAngleAxis;

    struct MathMatrix2Values
    {
        MathMatrix2Values() {}

        MathMatrix2Values
            (
            float _00, float _01,
            float _10, float _11
            ) :
            m00(_00), m01(_01),
            m10(_10), m11(_11)
        {
        }

        union
        {
            struct
            {
                float m00, m01;
                float m10, m11;
            };
            float m[2][2];
            float a[2 * 2];
        };
    };

    struct MathMatrix3Values
    {
        MathMatrix3Values() {}

        MathMatrix3Values
            (
            float _00, float _01, float _02,
            float _10, float _11, float _12,
            float _20, float _21, float _22
            ) :
            m00(_00), m01(_01), m02(_02),
            m10(_10), m11(_11), m12(_12),
            m20(_20), m21(_21), m22(_22)
        {
        }

        union
        {
            struct
            {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            };
            float m[3][3];
            float a[3 * 3];
        };
    };

    struct MathMatrix4Values
    {
        MathMatrix4Values() {}

        MathMatrix4Values
            (
            float _00, float _01, float _02, float _03,
            float _10, float _11, float _12, float _13,
            float _20, float _21, float _22, float _23,
            float _30, float _31, float _32, float _33
            ) :
            m00(_00), m01(_01), m02(_02), m03(_03),
            m10(_10), m11(_11), m12(_12), m13(_13),
            m20(_20), m21(_21), m22(_22), m23(_23),
            m30(_30), m31(_31), m32(_32), m33(_33)
        {
        }

        union
        {
            struct
            {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };
            float m[4][4];
            float a[4 * 4];
        };
    };

    class MathDecomposedAffineTransform
    {
    public:
        MathDecomposedAffineTransform();
        MathDecomposedAffineTransform(const MathAffineTransform& af);
        MathDecomposedAffineTransform(const MathMatrix4& m);

        void SetIdentity();
        void Set(const MathAffineTransform& af);
        void SetMatrix(const MathMatrix4& m);

        ValueOrError<void> GetMatrix(MathMatrix4& m) const;
        ValueOrError<void> GetInverse(MathDecomposedAffineTransform& inverse) const;

        struct Float4
        {
            float x, y, z, w;
        };

        typedef Float4 QuatPart;
        typedef Float4 VectorPart;

        struct Parts
        {
            VectorPart t; //Translation components
            QuatPart q; //Essential rotation
            QuatPart u; //Stretch rotation
            VectorPart k; //Stretch factors
            float f; //Sign of determinant
        };

        const Parts& GetParts() const;

        static ValueOrError<void> Interpolate
            (
            MathDecomposedAffineTransform& result,
            float t,
            const MathDecomposedAffineTransform& a,
            const MathDecomposedAffineTransform& b
            );

    private:
        Parts parts;
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename Matrix>
    void GetColumnsInCRowsMatrixData
        (
        typename Matrix::Scalar result[Matrix::RowsAtCompileTime][Matrix::ColsAtCompileTime],
        const Matrix& value
        )
    {
        //Destination will have the matrix columns in each of its rows

        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename Matrix::Scalar) * value.size());
    }

    template <typename Matrix>
    void GetRowsInCRowsMatrixData
        (
        typename Matrix::Scalar result[Matrix::RowsAtCompileTime][Matrix::ColsAtCompileTime],
        const Matrix& value
        )
    {
        //Destination will have the matrix rows in each of its rows

        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename Matrix::Scalar) * value.size());

        for (int row = 0; row < Matrix::RowsAtCompileTime; row++)
            for (int column = 0; column < row; column++)
                std::swap(result[row][column], result[column][row]);
    }

    template <typename Matrix>
    void GetColumnsFromCRowsMatrixData
        (
        Matrix& result,
        const typename Matrix::Scalar value[Matrix::RowsAtCompileTime][Matrix::ColsAtCompileTime]
        )
    {
        //Destination will have the matrix columns in each of its rows

        FINJIN_COPY_MEMORY(result.data(), value, sizeof(typename Matrix::Scalar) * result.size());
    }

    template <typename Matrix>
    void GetRowsFromCRowsMatrixData
        (
        Matrix& result,
        const typename Matrix::Scalar value[Matrix::RowsAtCompileTime][Matrix::ColsAtCompileTime]
        )
    {
        //Destination will have the matrix rows in each of its rows

        FINJIN_COPY_MEMORY(result.data(), value, sizeof(typename Matrix::Scalar) * result.size());

        result.transposeInPlace();
    }

    template <typename Struct, typename Matrix> //Struct will usually be MathMatrix4Values
    void GetColumnOrderMatrixData(Struct& result, const Matrix& value)
    {
        //Destination will have column 0 in its first set of values, column 1 in its second set, and so on
        //Same as calling GetMatrixAsArray((float*)&result, value, MatrixStorageOrder::COLUMNS);

        FINJIN_COPY_MEMORY(&result, value.data(), sizeof(typename Matrix::Scalar) * value.size());
    }

    template <typename Struct, typename Matrix> //Struct will usually be MathMatrix4Values
    void GetRowOrderMatrixData(Struct& result, const Matrix& value)
    {
        //Destination will have row 0 in its first set of values, row 1 in its second set, and so on
        //Same as calling GetMatrixAsArray((float*)&result, value, MatrixStorageOrder::ROWS);

        auto temp = value;
        temp.transposeInPlace();

        FINJIN_COPY_MEMORY(&result, temp.data(), sizeof(typename Matrix::Scalar) * temp.size());
    }

    template <typename Matrix>
    void GetMatrixAsArray
        (
        typename Matrix::Scalar* result,
        const Matrix& value,
        MatrixStorageOrder arrayStorageOrder = MatrixStorageOrder::COLUMNS
        )
    {
        if (arrayStorageOrder == MatrixStorageOrder::COLUMNS)
            FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename Matrix::Scalar) * value.size());
        else
        {
            auto temp = value;
            temp.transposeInPlace();

            FINJIN_COPY_MEMORY(result, temp.data(), sizeof(typename Matrix::Scalar) * value.size());
        }
    }

    template <typename Matrix>
    void SetMatrixFromArray
        (
        Matrix& result,
        const typename Matrix::Scalar* value,
        MatrixStorageOrder arrayStorageOrder = MatrixStorageOrder::COLUMNS
        )
    {
        FINJIN_COPY_MEMORY(result.data(), value, sizeof(typename Matrix::Scalar) * result.size());
        if (arrayStorageOrder == MatrixStorageOrder::ROWS)
            result.transposeInPlace();
    }

    template <typename Vector>
    void GetVectorAsArray(typename Vector::Scalar* result, const Vector& value)
    {
        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename Vector::Scalar) * value.size());
    }

    template <typename Vector>
    void SetVectorFromArray(Vector& result, const typename Vector::Scalar* value)
    {
        FINJIN_COPY_MEMORY(result.data(), value, sizeof(typename Vector::Scalar) * result.size());
    }

    inline MathVector3 SphericalToCartesian(float radius, Angle thetaAngle, Angle phiAngle)
    {
        auto theta = thetaAngle.ToRadiansValue();
        auto phi = phiAngle.ToRadiansValue();
        return MathVector3(radius * sinf(phi) * cosf(theta), radius * cosf(phi), radius * sinf(phi) * sinf(theta));
    }

    inline float RandF()
    {
        //Returns random float in [0, 1)
        return RoundToFloat(rand()) / (float)RAND_MAX;
    }

    inline float RandF(float a, float b)
    {
        //Returns random float in [a, b)
        return a + RandF() * (b - a);
    }

    inline int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

    inline Angle RandAngle()
    {
        return Radians(RandF(0.0f, 2.0f * FINJIN_PI));
    }

    template <typename T>
    inline T Lerp(const T& a, const T& b, float t)
    {
        return a + (b - a) * t;
    }

    inline Angle AngleFromXY(float x, float y)
    {
        //Returns the polar angle of the point (x,y) in [0, 2*PI).
        float theta = 0.0f;

        if (x >= 0.0f)
        {
            //Quadrant I or IV

            //If x = 0, then atanf(y/x) = +pi/2 if y > 0
            //               atanf(y/x) = -pi/2 if y < 0
            theta = atanf(y / x); //in [-pi/2, +pi/2]

            if (theta < 0.0f)
                theta += 2.0f * FINJIN_PI; //in [0, 2*pi).
        }
        else
        {
            //Quadrant II or III
            theta = atanf(y / x) + FINJIN_PI; //in [0, 2*pi).
        }

        return Radians(theta);
    }

    inline bool Vector3Greater(const MathVector3& v1, const MathVector3& v2)
    {
        return v1(0) > v2(0) && v1(1) > v2(1) && v1(2) > v2(2);
    }

    inline bool Vector3Less(const MathVector3& v1, const MathVector3& v2)
    {
        return v1(0) < v2(0) && v1(1) < v2(1) && v1(2) < v2(2);
    }

    inline MathVector3 RandUnitVector3()
    {
        return SphericalToCartesian(1.0f, RandAngle(), RandAngle());
    }

    inline MathVector3 RandHemisphereUnitVector3(MathVector3 n, size_t* iterationCount = nullptr)
    {
        for (size_t i = 0; i < (size_t)-1; i++)
        {
            auto v = RandUnitVector3();
            if (n.dot(v) >= 0.0f)
            {
                if (iterationCount != nullptr)
                    *iterationCount = i;
                return v;
            }
        }

        return MathVector3(1.0f, 0.0f, 0.0f);
    }

} }
