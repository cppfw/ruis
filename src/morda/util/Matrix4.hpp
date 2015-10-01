/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <algorithm>
#include <iostream>

#include <utki/debug.hpp>

#include "../config.hpp"

#include "Vector4.hpp"



namespace morda{



template <class T> class Vector2;
template <class T> class Vector3;
template <class T> class Quaternion;



/**
 * @brief 4x4 matrix template class.
 * Note, that this matrix class stores elements in memory column by column.
 * This is the same way as OpenGL matrices are stored in memory.
 * This means easy use of this class with OpenGL.
 */
template <typename T> class Matrix4{
public:
	/**
	 * @brief 0th column of the matrix.
	 */
	Vector4<T> c0;
	
	/**
	 * @brief 1st column of the matrix.
	 */
	Vector4<T> c1;
	
	/**
	 * @brief 2nd column of the matrix.
     */
	Vector4<T> c2;
	
	/**
	 * @brief 3rd column of the matrix.
     */
	Vector4<T> c3;


	
	/**
	 * @brief Default constructor.
	 * NOTE: it does not initialize the matrix with any values.
	 * Matrix elements are undefined after the matrix is created with this constructor.
	 */
	Matrix4()noexcept{}
	
	
	
	/**
	 * @brief Construct initialized matrix.
	 * Creates a matrix and initializes its columns by the given values.
     * @param column0 - 0th column of the matrix.
	 * @param column1 - 1st column of the matrix.
	 * @param column2 - 2nd column of the matrix.
	 * @param column3 - 3rd column of the matrix.
     */
	Matrix4(
			const Vector4<T>& column0,
			const Vector4<T>& column1,
			const Vector4<T>& column2,
			const Vector4<T>& column3
		)noexcept :
			c0(column0),
			c1(column1),
			c2(column2),
			c3(column3)
	{}



	/**
	 * @brief Construct rotation matrix.
	 * Constructs matrix and initializes it to a rotation matrix from given unit quaternion.
     * @param quat - unit quaternion defining the rotation.
     */
	Matrix4(const Quaternion<T>& quat)noexcept;

	

	template <class TT> Matrix4(const Matrix4<TT>& m) :
			c0(m.c0),
			c1(m.c1),
			c2(m.c2),
			c3(m.c3)
	{}
	
	

	Matrix4(const Matrix4&) = default;
	Matrix4& operator=(const Matrix4&) = default;

	

	/**
	 * @brief returns reference to specified column.
	 * Returns reference to the matrix column indicated by the argument.
	 * @code
	 * Matrix4 m;
	 * m[0][0] = 1;//assign 1 to element at row 0 column 0
	 * m[3][2] = 3;//assign 3 to element at row 2 column 3
	 * float elem = m[4][3];//assign value at row 3 column 4 of the matrix to variable 'elem'
	 * @endcode
	 * @param col - column number, must be from 0 to 3.
	 * @return reference to the matrix column indicated by the argument.
	 */
	Vector4<T>& operator[](unsigned col)noexcept{
		ASSERT(col < 4)
		return (&this->c0)[col];
	}

	/**
	 * @brief returns reference to specified column.
	 * Constant variant of operator[].
	 * @param col - column number, must be from 0 to 3.
	 * @return reference to the matrix column indicated by the argument.
	 */
	const Vector4<T>& operator[](unsigned col)const noexcept{
		ASSERT(col < 4)
		return (&this->c0)[col];
	}

	/**
	 * @brief Transform vector by matrix.
	 * Multiply vector V by this matrix M from the right (M * V).
	 * i.e. transform vector with this transformation matrix.
	 * @param vec - vector to transform.
     * @return Transformed vector.
     */
	Vector2<T> operator*(const Vector2<T>& vec)const noexcept;
	
	/**
	 * @brief Transform vector by matrix.
	 * Multiply vector V by this matrix M from the right (M * V).
	 * i.e. transform vector with this transformation matrix.
	 * @param vec - vector to transform.
     * @return Transformed vector.
     */
	Vector3<T> operator*(const Vector3<T>& vec)const noexcept;

	
	
	/**
	 * @brief Transform vector by matrix.
	 * Multiply vector V by this matrix M from the right (M * V).
	 * i.e. transform vector with this transformation matrix.
	 * @param vec - vector to transform.
     * @return Transformed vector.
     */
	Vector4<T> operator*(const Vector4<T>& vec)const noexcept;
	
	
	
	/**
	 * @brief Get matrix row.
	 * Constructs a Vector4 holding requested row of the matrix.
	 * @param rowNum - row number to get, must be from 0 to 3.
     * @return Vector4 representing the row of this matrix.
     */
	Vector4<T> row(unsigned rowNum)const noexcept{
		ASSERT(rowNum < 4)
		return Vector4<T>(this->c0[rowNum], this->c1[rowNum], this->c2[rowNum], this->c3[rowNum]);
	}
	
	
	
	/**
	 * @brief Multiply by matrix from the right.
	 * Calculate result of this matrix M multiplied by another matrix K from the right (M * K).
	 * @param matr - matrix to multiply by (matrix K).
     * @return New matrix as a result of matrices product.
     */
	Matrix4 operator*(const Matrix4& matr)const noexcept{
		return Matrix4(
				Vector4<T>(this->row(0) * matr[0], this->row(1) * matr[0], this->row(2) * matr[0], this->row(3) * matr[0]),
				Vector4<T>(this->row(0) * matr[1], this->row(1) * matr[1], this->row(2) * matr[1], this->row(3) * matr[1]),
				Vector4<T>(this->row(0) * matr[2], this->row(1) * matr[2], this->row(2) * matr[2], this->row(3) * matr[2]),
				Vector4<T>(this->row(0) * matr[3], this->row(1) * matr[3], this->row(2) * matr[3], this->row(3) * matr[3])
			);
	}
	
	
	
	//NOTE: operator=() will be generated by compiler



	/**
	 * @brief Transpose matrix.
	 */
	Matrix4& transpose()noexcept{
		std::swap(this->c0[1], this->c1[0]);
		std::swap(this->c0[2], this->c2[0]);
		std::swap(this->c0[3], this->c3[0]);
		
		std::swap(this->c1[2], this->c2[1]);
		std::swap(this->c1[3], this->c3[1]);
		
		std::swap(this->c2[3], this->c3[2]);
		return (*this);
	}



	/**
	 * @brief Multiply by matrix from the right.
	 * Multiply this matrix M by another matrix K from the right (M  = M * K).
     * @return reference to this matrix object.
     */
	Matrix4& operator*=(const Matrix4& matr)noexcept{
		return this->operator=(this->operator*(matr));
	}
	
	
	
	/**
	 * @brief Multiply by matrix from the right.
	 * Multiply this matrix M by another matrix K from the right (M  = M * K).
	 * This is the same as operator*=().
	 * @param matr - matrix to multiply by.
	 * @return reference to this matrix object.
	 */
	Matrix4& rightMulBy(const Matrix4& matr)noexcept{
		return this->operator*=(matr);
	}



	/**
	 * @brief Multiply by matrix from the left.
	 * Multiply this matrix M by another matrix K from the left (M  = K * M).
	 * @param matr - matrix to multiply by.
	 * @return reference to this matrix object.
	 */
	Matrix4& leftMulBy(const Matrix4& matr)noexcept{
		return this->operator=(matr.operator*(*this));
	}



	/**
	 * @brief Initialize this matrix with identity matrix.
	 */
	Matrix4& identity()noexcept{
		this->c0 = Vector4<T>(1, 0, 0, 0);
		this->c1 = Vector4<T>(0, 1, 0, 0);
		this->c2 = Vector4<T>(0, 0, 1, 0);
		this->c3 = Vector4<T>(0, 0, 0, 1);
		return (*this);
	}
	
	
	
	/**
	 * @brief Multiply current matrix by frustum matrix.
	 * Multiplies this matrix M by frustum matrix S from the right (M = M * S).
	 * Parameters are identical to glFrustum() function from OpenGL.
	 * @param left - left vertical clipping plane.
	 * @param right - right vertical clipping plane.
	 * @param bottom - bottom horizontal clipping plane.
	 * @param top - top horizontal clipping plane.
	 * @param nearVal - distance to near depth clipping plane. Must be positive.
	 * @param farVal - distance to the far clipping plane. Must be positive.
	 * @return reference to this matrix instance.
	 */
	Matrix4& frustum(T left, T right, T bottom, T top, T nearVal, T farVal)noexcept{
		T w = right - left;
		ASSERT(w != 0)
		
		T h = top - bottom;
		ASSERT(h != 0)
				
		T d = farVal - nearVal;
		ASSERT(d != 0)
		
		Matrix4 f;
		f[0][0] = 2 * nearVal / w;
		f[1][0] = 0;
		f[2][0] = (right + left) / w;
		f[3][0] = 0;
		
		f[0][1] = 0;
		f[1][1] = 2 * nearVal / h;
		f[2][1] = (top + bottom) / h;
		f[3][1] = 0;
		
		f[0][2] = 0;
		f[1][2] = 0;
		f[2][2] = -(farVal + nearVal) / d;
		f[3][2] = -2 * farVal * nearVal / d;
		
		f[0][3] = 0;
		f[1][3] = 0;
		f[2][3] = -1;
		f[3][3] = 0;
				
		return this->rightMulBy(f);
	}
	
	
	
	/**
	 * @brief Set this matrix to be a rotation matrix.
	 * Sets this matrix to a matrix representing a rotation defined by a unit quaternion.
     * @param quat - unit quaternion defining the rotation.
     * @return Reference to this matrix object.
     */
	Matrix4& set(const Quaternion<T>& quat)noexcept;
	
	
	
	/**
	 * @brief Multiply current matrix by scale matrix.
	 * Multiplies this matrix M by scale matrix S from the right (M = M * S).
	 * Scaling factor in z direction is 1.
	 * @param x - scaling factor in x direction.
	 * @param y - scaling factor in y direction.
	 * @return reference to this matrix instance.
	 */
	Matrix4& scale(T x, T y)noexcept{
		//update 0th column
		this->c0 *= x;
		
		//update 1st column
		this->c1 *= y;

		//NOTE: 2nd and 3rd columns remain unchanged
		return (*this);
	}
	


	/**
	 * @brief Multiply current matrix by scale matrix.
	 * Multiplies this matrix M by scale matrix S from the right (M = M * S).
	 * @param x - scaling factor in x direction.
	 * @param y - scaling factor in y direction.
	 * @param z - scaling factor in z direction.
	 * @return reference to this matrix instance.
	 */
	Matrix4& scale(T x, T y, T z)noexcept{
		//update 0th and 1st columns
		this->scale(x, y);

		//update 2nd column
		this->c2 *= z;

		//NOTE: 3rd column remains unchanged
		return (*this);
	}
	
	
	
	/**
	 * @brief Multiply current matrix by scale matrix.
	 * Multiplies this matrix M by scale matrix S from the right (M = M * S).
	 * @param s - scaling factor to be applied in all 3 directions (x, y and z).
	 * @return reference to this matrix instance.
	 */
	Matrix4& scale(T s)noexcept{
		return this->Scale(s, s, s);
	}
	
	
	
	/**
	 * @brief Multiply current matrix by scale matrix.
	 * Multiplies this matrix M by scale matrix S from the right (M = M * S).
	 * @param s - vector of scaling factors in x and y directions, scaling factor in z direction is 1.
	 * @return reference to this matrix instance.
	 */
	Matrix4& scale(const Vector2<T>& s)noexcept;
	
	
	
	/**
	 * @brief Multiply current matrix by scale matrix.
	 * Multiplies this matrix M by scale matrix S from the right (M = M * S).
	 * @param s - vector of scaling factors in x, y and z directions.
	 * @return reference to this matrix instance.
	 */
	Matrix4& scale(const Vector3<T>& s)noexcept;
	
	
	
	/**
	 * @brief Multiply this matrix by translation matrix.
	 * Multiplies this matrix M by translation matrix T from the right (M = M * T).
	 * Translation only occurs in x-y plane, no translation in z direction,
	 * i.e. z component of translation vector is assumed to be 0.
	 * @param x - x component of translation vector.
	 * @param y - y component of translation vector.
	 * @return reference to this matrix object.
	 */
	Matrix4& translate(T x, T y)noexcept{
		//NOTE: 0th, 1st and 2nd columns remain unchanged

		//calculate 3rd column
		this->c3 = this->c0 * x + this->c1 * y + this->c3;

		return (*this);
	}
	
	
	
	/**
	 * @brief Multiply this matrix by translation matrix.
	 * Multiplies this matrix M by translation matrix T from the right (M = M * T).
	 * @param x - x component of translation vector.
	 * @param y - y component of translation vector.
	 * @param z - z component of translation vector.
	 * @return reference to this matrix object.
	 */
	Matrix4& translate(T x, T y, T z)noexcept{
		//NOTE: 0th, 1st and 2nd columns remain unchanged
		this->translate(x, y);

		this->c3 += this->c2 * z;

		return (*this);
	}
	
	
	
	/**
	 * @brief Multiply this matrix by translation matrix.
	 * Multiplies this matrix M by translation matrix T from the right (M = M * T).
	 * Translation only occurs in x-y plane, no translation in z direction,
	 * i.e. z component of translation vector is assumed being 0.
	 * @param t - translation vector.
	 * @return reference to this matrix object.
	 */
	Matrix4& translate(const Vector2<T>& t)noexcept;
	
	
	
	/**
	 * @brief Multiply this matrix by translation matrix.
	 * Multiplies this matrix M by translation matrix T from the right (M = M * T)
	 * @param t - translation vector.
	 * @return reference to this matrix object.
	 */
	Matrix4& translate(const Vector3<T>& t)noexcept;



	/**
	 * @brief Multiply this matrix by rotation matrix.
	 * Multiplies this matrix M by rotation matrix R from the right (M = M * R).
	 * @param q - unit quaternion, representing the rotation.
	 * @return reference to this matrix object.
	 */
	Matrix4& rotate(const Quaternion<T>& q)noexcept;



	/**
	 * @brief Multiply this matrix by rotation matrix.
	 * Multiplies this matrix M by rotation matrix R from the right (M = M * R).
	 * @param rot - vector, representing the rotation. The vector direction
	 *              defines the axis of rotation, vector length defines
	 *              the angle of rotation in radians.
	 * @return reference to this matrix object.
	 */
	Matrix4& rotate(const Vector3<T>& rot)noexcept;



	/**
	 * @brief Multiply this matrix by rotation matrix.
	 * Multiplies this matrix M by rotation matrix R from the right (M = M * R).
	 * Rotation is done around (0, 0, 1) axis by given number of radians.
	 * Positive direction of rotation is determined by a right-hand rule.
	 * @param rot - the angle of rotation in radians.
	 * @return reference to this matrix object.
	 */
	Matrix4& rotate(T rot)noexcept;

	friend std::ostream& operator<<(std::ostream& s, const Matrix4<T>& mat){
		s << "\n";
		s << "\t/" << mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] << "\\" << std::endl;
		s << "\t|" << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << "|" << std::endl;
		s << "\t|" << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << "|" << std::endl;
		s << "\t\\" << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << "/";
		return s;
	};
};//~class



}//~namespace



#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"



namespace morda{

//=================================
// inline functions implementation
//=================================

template <class T> Vector2<T> Matrix4<T>::operator*(const Vector2<T>& vec)const noexcept{
	return Vector2<T>(
			this->row(0) * vec,
			this->row(1) * vec
		);
}



template <class T> Vector3<T> Matrix4<T>::operator*(const Vector3<T>& vec)const noexcept{
	return Vector3<T>(
			this->row(0) * vec,
			this->row(1) * vec,
			this->row(2) * vec
		);
}



template <class T> Vector4<T> Matrix4<T>::operator*(const Vector4<T>& vec)const noexcept{
	return Vector4<T>(
			this->row(0) * vec,
			this->row(1) * vec,
			this->row(2) * vec,
			this->row(3) * vec
		);
}



template <class T> Matrix4<T>& Matrix4<T>::scale(const Vector3<T>& s)noexcept{
	return this->Scale(s.x, s.y, s.z);
}



template <class T> Matrix4<T>& Matrix4<T>::scale(const Vector2<T>& s)noexcept{
	return this->scale(s.x, s.y);
}



template <class T> Matrix4<T>& Matrix4<T>::translate(const Vector2<T>& t)noexcept{
	return this->translate(t.x, t.y);
}



template <class T> Matrix4<T>& Matrix4<T>::translate(const Vector3<T>& t)noexcept{
	return this->Translate(t.x, t.y, t.z);
}



template <class T> Matrix4<T>& Matrix4<T>::rotate(const Quaternion<T>& q)noexcept{
	return this->rightMulBy(Matrix4<T>(q));
}



template <class T> Matrix4<T>& Matrix4<T>::rotate(const Vector3<T>& rot)noexcept{
	return this->Rotate(Quaternion<T>(rot));
}



template <class T> Matrix4<T>& Matrix4<T>::rotate(T rot)noexcept{
	return this->Rotate(Vector3<T>(0, 0, rot));
}



template <class T> Matrix4<T>::Matrix4(const Quaternion<T>& quat)noexcept{
	this->set(quat);
}



template <class T> Matrix4<T>& Matrix4<T>::set(const Quaternion<T>& quat)noexcept{
	// After about 300 trees murdered and 20 packs of chalk depleted, the
	// mathematicians came up with these equations for a quaternion to matrix conversion:
	//     /  1-(2y^2+2z^2)   2xy-2zw         2xz+2yw         0   \
	// M = |  2xy+2zw         1-(2x^2+2z^2)   2yz-2xw         0   |
	//     |  2xz-2yw         2zy+2xw         1-(2x^2+2y^2)   0   |
	//     \  0               0               0               1   /

	//First column
	this->c0[0] = T(1) - T(2) * (utki::pow2(quat.y) + utki::pow2(quat.z));
	this->c0[1] = T(2) * (quat.x * quat.y + quat.z * quat.w);
	this->c0[2] = T(2) * (quat.x * quat.z - quat.y * quat.w);
	this->c0[3] = T(0);

	//Second column
	this->c1[0] = T(2) * (quat.x * quat.y - quat.z * quat.w);
	this->c1[1] = T(1) - T(2) * (utki::pow2(quat.x) + utki::pow2(quat.z));
	this->c1[2] = T(2) * (quat.z * quat.y + quat.x * quat.w);
	this->c1[3] = T(0);

	//Third column
	this->c2[0] = T(2) * (quat.x * quat.z + quat.y * quat.w);
	this->c2[1] = T(2) * (quat.y * quat.z - quat.x * quat.w);
	this->c2[2] = T(1) - T(2) * (utki::pow2(quat.x) + utki::pow2(quat.y));
	this->c2[3] = T(0);

	//Fourth column
	this->c3[0] = T(0);
	this->c3[1] = T(0);
	this->c3[2] = T(0);
	this->c3[3] = T(1);
	
	return *this;
}



//=====================
// Convenient typedefs
//=====================

typedef Matrix4<float> Matr4f;
static_assert(sizeof(Matr4f) == sizeof(float) * 4 * 4, "size mismatch");

typedef Matrix4<double> Matr4d;
static_assert(sizeof(Matr4d) == sizeof(double) * 4 * 4, "size mismatch");

typedef Matrix4<real> Matr4r;
static_assert(sizeof(Matr4r) == sizeof(real) * 4 * 4, "size mismatch");


}//~namespace
