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


//Includes---------------------------------------------------------------------
#include "finjin/common/Angle.hpp"
#include "finjin/common/ValueOrError.hpp"
#include <Eigen/Eigen>


//Macros-----------------------------------------------------------------------
#define FINJIN_PI 3.1415926535f


//Classes/functions------------------------------------------------------------
namespace Finjin { namespace Common {

    typedef Eigen::Matrix2f MathMatrix22;
    typedef Eigen::Matrix3f MathMatrix33;
    typedef Eigen::Matrix4f MathMatrix44;

    typedef Eigen::Vector2f MathVector2;
    typedef Eigen::Vector3f MathVector3;
    typedef Eigen::Vector4f MathVector4;
    
    typedef Eigen::Quaternionf MathQuaternion;
    
    typedef Eigen::Affine3f MathAffineTransform;
    
    typedef Eigen::Translation3f MathTranslation;
    typedef Eigen::AlignedScaling3f MathScaling;

    typedef Eigen::AngleAxisf MathAngleAxis;

    class MathDecomposedAffineTransform
    {
    public:
        MathDecomposedAffineTransform();
        MathDecomposedAffineTransform(const MathAffineTransform& af);
        MathDecomposedAffineTransform(const MathMatrix44& m);

        void SetIdentity();
        void Set(const MathAffineTransform& af);
        void SetMatrix(const MathMatrix44& m);
        
        ValueOrError<void> GetMatrix(MathMatrix44& m) const;
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
            QuatPart q;	//Essential rotation
            QuatPart u;	//Stretch rotation
            VectorPart k; //Stretch factors
            float f; //Sign of determinant
        };

        const Parts& GetParts() const;

        static ValueOrError<void> Interpolate(MathDecomposedAffineTransform& result, float t, const MathDecomposedAffineTransform& a, const MathDecomposedAffineTransform& b);        

    private:
        Parts parts;
    };

    template <typename T>
    void GetRowMajorSquareMatrixData(typename T::Scalar result[T::RowsAtCompileTime][T::ColsAtCompileTime], const T& value)
    {
        //Destination is row major 2D array

        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename T::Scalar) * value.size());
    }

    template <typename T, typename Matrix>
    void GetRowMajorSquareMatrixData(T& result, const Matrix& value)
    {
        //Destination is row major

        FINJIN_COPY_MEMORY(&result, value.data(), sizeof(typename Matrix::Scalar) * value.size());
    }

    template <typename T>
    void SetRowMajorSquareMatrixData(T& value, const typename T::Scalar data[T::RowsAtCompileTime][T::ColsAtCompileTime])
    {
        //Source is row major
        
        FINJIN_COPY_MEMORY(value.data(), data, sizeof(typename T::Scalar) * value.size());
    }

    template <typename T>
    void GetColumnMajorSquareMatrixData(typename T::Scalar result[T::RowsAtCompileTime][T::ColsAtCompileTime], const T& value)
    {
        //Destination is column major 2D array

        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename T::Scalar) * value.size());
        
        for (int row = 0; row < T::RowsAtCompileTime; row++)
            for (int col = 0; col < row; col++)
                std::swap(result[row][col], result[col][row]);
    }

    template <typename T, typename Matrix>
    void GetColumnMajorSquareMatrixData(T& result, const Matrix& value)
    {
        //Destination is column major
        auto temp = value;
        temp.transposeInPlace();

        FINJIN_COPY_MEMORY(&result, temp.data(), sizeof(typename Matrix::Scalar) * temp.size());
    }

    template <typename T>
    void SetColumnMajorSquareMatrixData(T& value, const typename T::Scalar data[T::RowsAtCompileTime][T::ColsAtCompileTime])
    {
        //Source is column major

        FINJIN_COPY_MEMORY(value.data(), data, sizeof(typename T::Scalar) * value.size());
        
        value.transposeInPlace();
    }

    template <typename T>
    void GetMathData(typename T::Scalar* result, const T& value)
    {
        FINJIN_COPY_MEMORY(result, value.data(), sizeof(typename T::Scalar) * value.size());
    }

    template <typename T>
    void SetMathData(const T& value, const typename T::Scalar* data)
    {
        FINJIN_COPY_MEMORY(value.data(), data, sizeof(typename T::Scalar) * value.size());
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
