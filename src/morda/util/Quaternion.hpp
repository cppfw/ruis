/* The MIT License:

Copyright (c) 2008-2012 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <cmath> //TODO: ?
#include <cstdlib> //TODO: ?
#include <algorithm> //TODO: ?

#ifdef DEBUG
#	include <iostream>
#endif

#include <ting/debug.hpp>//TODO: ?
#include <ting/types.hpp>//TODO: ?
#include <ting/math.hpp>//TODO: ?
#include <ting/Exc.hpp>//TODO: ?

//TODO: add throw() where needed

namespace morda{



/**
 * @brief Quaternion template class.
 */
template <typename T> class Quaternion{
public:
	/**
	 * @brief x component.
	 */
	T x;

	/**
	 * @brief y component.
	 */
	T y;
	
	/**
	 * @brief z component.
	 */
	T z;

	/**
	 * @brief w component.
	 */
	T w;



	/**
	 * @brief Create quaternion with given components.
	 * @param qx - x component.
	 * @param qy - y component.
	 * @param qz - z component.
	 * @param qw - w component.
	 */
	Quaternion(T qx, T qy, T qz, T qw) :
			x(qx),
			y(qy),
			z(qz),
			w(qw)
	{}



	/**
	 * @brief Construct rotation quaternion.
	 * Constructs a quaternion representing rotation (unit quaternion).
	 * Rotation is given by 3 dimensional vector, whose direction defines the
	 * axis about which rotation is done and its magnitude defines the angle of
	 * rotation in radians.
	 * @param axis - vector which defines the rotation.
	 */
	Quaternion(const Vector3<T>& axis){
		T mag = axis.Magnitude();//magnitude is a rotation angle
		if(mag != 0){
			Vector3<T> a = axis;
			a /= mag;//normalize axis
			this->InitRot(a.x, a.y, a.z, mag);
		}else
			this->Identity();
	}



	/**
	 * @brief Default constructor.
	 * Note, that it does not initialize quaternion components,
	 * right after creation the components are undefined.
	 */
	inline Quaternion(){}



	/**
	 * @brief Complex conjugate of this quaternion.
	 * Note, complex conjugate of quaternion (x, y, z, w) is (-x, -y, -z, w).
	 * @return quaternion instance which is a complex conjugate of this quaternion.
	 */
	inline Quaternion operator!()const{
		return Quaternion(-this->x, -this->y, -this->z, this->w);
	}



	//TODO: doxygen
	inline Quaternion& operator+=(const Quaternion& q){
		this->x += q.x;
		this->y += q.y;
		this->z += q.z;
		this->w += q.w;
		return (*this);
	}



	//TODO: doxygen
	inline Quaternion operator+(const Quaternion& q)const{
		return Quaternion(this->x + q.x, this->y + q.y, this->z + q.z, this->w + q.w);
	}



	//TODO: doxygen
	inline Quaternion& operator=(const Quaternion& q){
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
		return (*this);
	}



	/**
	 * @brief Multiply by scalar and assign.
	 * Multiplies this quaternion by scalar and assigns the result to this quaternion instance.
	 * @param s - scalar value to multiply by.
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& operator*=(T s){
		this->x *= s;
		this->y *= s;
		this->z *= s;
		this->w *= s;
		return (*this);
	}



	/**
	 * @brief Multiply by scalar.
	 * @param s - scalar value to multiply by.
	 * @return resulting quaternion instance.
	 */
	inline Quaternion operator*(T s)const{
		return (Quaternion(*this) *= s);
	}



	/**
	 * @brief Divide by scalar and assign.
	 * Divide this quaternion by scalar and assigns the result to this quaternion instance.
	 * @param s - scalar value to divide by.
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& operator/=(T s){
		this->x /= s;
		this->y /= s;
		this->z /= s;
		this->w /= s;
		return (*this);
	}



	/**
	 * @brief Divide by scalar.
	 * @param s - scalar value to divide by.
	 * @return resulting quaternion instance.
	 */
	inline Quaternion operator/(T s)const{
		return (Quaternion(*this) /= s);
	}



	/**
	 * @brief Dot product of quaternions.
	 * Dot product of two quaternions (x1, y1, z1, w1) and
	 * (x2, y2, z2, w2) is a scalar calculated as follows
	 * x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2
	 * @return result of the dot product.
	 */
	inline T operator*(const Quaternion& q)const{
		return this->x * q.x + this->y * q.y + this->z * q.z + this->w * q.w;
	}



	/**
	 * @brief Multiply by quaternion and assign.
	 * Multiplies this quaternion by another quaternion from the right
	 * (quaternions multiplication is not associative) and assignes the
	 * result to this quaternion instance.
	 * @param q - quaternion to multiply by.
	 * @return reference to this quaternion instance.
	 */
	Quaternion& operator%=(const Quaternion& q){
		T a = (this->w + this->x) * (q.w + q.x);
		T b = (this->z - this->y) * (q.y - q.z);
		T c = (this->x - this->w) * (q.y + q.z);
		T d = (this->y + this->z) * (q.x - q.w);
		T e = (this->x + this->z) * (q.x + q.y);
		T f = (this->x - this->z) * (q.x - q.y);
		T g = (this->w + this->y) * (q.w - q.z);
		T h = (this->w - this->y) * (q.w + q.z);

		this->x = a - (e + f + g + h) * 0.5f;
		this->y = -c + (e - f + g - h) * 0.5f;
		this->z = -d + (e - f - g + h) * 0.5f;
		this->w = b + (-e - f + g + h) * 0.5f;
		return (*this);
	}



	/**
	 * @brief Multiply by quaternion.
	 * Multiplies this quaternion by another quaternion from the right
	 * (quaternions multiplication is not associative).
	 * @param q - quaternion to multiply by.
	 * @return resulting quaternion instance.
	 */
	//multiplication of quaternions
	inline Quaternion operator%(const Quaternion& q)const{
		return (Quaternion(*this) %= q);
	}



	/**
	 * @brief Initialize with identity quaternion.
	 * Note, identity quaternion is (0, 0, 0, 1).
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& Identity(){
		this->x = T(0);
		this->y = T(0);
		this->z = T(0);
		this->w = T(1);
		return *this;
	}



	/**
	 * @brief Complex conjugate this quaternion.
	 * Note, complex conjugate of quaternion (x, y, z, w) is (-x, -y, -z, w).
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& Conjugate(){
		*this = !(*this);
		return *this;
	}



	/**
	 * @brief Negate this quaternion.
	 * Note, negating quaternion means changing the sign of its every component.
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& Negate(){
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		this->w = -this->w;
		return *this;
	}



	/**
	 * @brief Calculate power 2 from quaternion magnitude.
	 * @return power 2 from magnitude.
	 */
	inline T MagPow2()const{
		return (*this) * (*this);
	}



	/**
	 * @brief Calculate quaternion magnitude.
	 * @return quaternion magnitude.
	 */
	inline T Magnitude()const{
		return T(sqrt(this->MagPow2()));
	}



	/**
	 * @brief Normalize quaternion.
	 * Note, after normalization, the quaternion becomes a unit quaternion.
	 * @return reference to this quaternion instance.
	 */
	inline Quaternion& Normalize(){
		return (*this) /= Magnitude();
	}



	//TODO: doxygen
	//TODO: consider removing this function moving its functional to corresponding constructor.
	//Initialize this with rotation unit quaternion from axis (normalized) and an angle
	inline void InitRot(T xx, T yy, T zz, T angle){
		T sina2 = T(sin(angle / 2));
		this->w = T(cos(angle / 2));
		this->x = xx * sina2;
		this->y = yy * sina2;
		this->z = zz * sina2;
	}



	//TODO: doxygen
	//TODO:consider removing this function
	//multiply this quaternion by unit rotation quaternion
	//from the left
	//TODO: check how this function relates with rotation matrixes multiplication (left-right)
	//      need only "mult from the right" function
	Quaternion& RotateLeft(Vector3<T> axis, T angle){
		Quaternion r;
		r.InitRot(axis.x, axis.y, axis.z, angle);
		return (*this) = r % (*this);
	}



	//TODO: doxygen
	//Create 4x4 OpenGL like rotation matrix from this quaternion
	//ARGS: m - matrix to fill
	//RETURNS: return a reference to m
	//TODO: move this functionality to  Matrix4::Matrix4(const Quaternion& q), i.e. create a contructor
	Matrix4<T>& CreateMatrix4(Matrix4<T>& m)const{
		// After about 300 trees murdered and 20 packs of chalk depleted, the
		// mathematicians came up with these equations for a quaternion to matrix converion:
		//   /  1-(2y^2+2z^2)   2xy+2zw         2xz-2yw         0   \T
		// M=|  2xy-2zw         1-(2x^2+2z^2)   2zy+2xw         0   |
		//   |  2xz+2yw         2yz-2xw         1-(2x^2+2y^2)   0   |
		//   \  0               0               0               1   /

		//First column
		m[0][0] = T(1) - T(2) * (ting::math::Pow2(this->y) + ting::math::Pow2(this->z));
		m[0][1] = T(2) * (this->x * this->y + this->z * this->w);
		m[0][2] = T(2) * (this->x * this->z - this->y * this->w);
		m[0][3] = T(0);

		//Second column
		m[1][0] = T(2) * (this->x * this->y - this->z * this->w);
		m[1][1] = T(1) - T(2) * (ting::math::Pow2(this->x) + ting::math::Pow2(this->z));
		m[1][2] = T(2) * (this->z * this->y + this->x * this->w);
		m[1][3] = T(0);

		//Third column
		m[2][0] = T(2) * (this->x * this->z + this->y * this->w);
		m[2][1] = T(2) * (this->y * this->z - this->x * this->w);
		m[2][2] = T(1) - T(2) * (ting::math::Pow2(this->x) + ting::math::Pow2(this->y));
		m[2][3] = T(0);

		//Fourth column
		m[3][0] = T(0);
		m[3][1] = T(0);
		m[3][2] = T(0);
		m[3][3] = T(1);
		return m;
	}



	//TODO: doxygen
	//--||--||--
	Matrix4<T> ToMatrix4()const{
		Matrix4<T> m;
		this->CreateMatrix4(m);
		return m;
	}


	
	//TODO: doxygen
	//Spherical linear interpolation.
	//This quaternion = SLERP(q1,q2,t), t from [0;1].
	//SLERP(q1,q2,t) = q1*sin((1-t)*alpha)/sin(alpha)+q2*sin(t*alpha)/sin(alpha),
	//where cos(alpha) = (q1,q2) (dot product of normalized quaternions q1 and q2).
	//It is assumed that quaternions are normalized!
	void Slerp(const Quaternion& q1, const Quaternion& q2, T t){
		//Since quaternions are normalized the cosine of the angle alpha
		//between quaternions are equal to their dot product.
		T cosalpha = q1 * q2;

		//If the dot product is less than 0, the angle alpha between quaternions
		//is greater than 90 degrees. Then we negate second quaternion to make alpha
		//be less than 90 degrees. It is possible since normalized quaternions
		//q and -q represent the same rotation!
		if(cosalpha < T(0)){
			//Negate the second quaternion and the result of the dot product (i.e. cos(alpha))
			q2.Negate();
			cosalpha = -cosalpha;
		}

		//interpolation done by the following general formula:
		//RESULT=q1*sc1(t)+q2*sc2(t).
		//Where sc1,sc2 called interpolation scales.
		T sc1, sc2;//Define variables for scales for interpolation

		//Check if the angle alpha between the 2 quaternions is big enough
		//to make SLERP. If alpha is small then we do a simple linear
		//interpolation between quaternions instead of SLERP!
		//It is also used to avoid divide by zero since sin(0)=0 !
		//We made threshold for cos(alpha)>0.9f (if cos(alpha)==1 then alpha=0).
		if(cosalpha > T(0.9f)){
			//Get the angle alpha between the 2 quaternions, and then store the sin(alpha)
			T alpha = T(acos(cosalpha));
			T sinalpha = T(sin(alpha)); //TODO: use ting::Sin etc.

			//Calculate the scales for q1 and q2, according to the angle and it's sine value
			sc1 = T( sin((1 - t) * alpha) / sinalpha );
			sc2 = T( sin(t * alpha) / sinalpha );
		}else{
			sc1 = (1 - t);
			sc2 = t;
		}

		// Calculate the x, y, z and w values for the interpolated quaternion.
		(*this) = q1 * sc1 + q2 * sc2;
	}

#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Quaternion<T>& quat){
		s << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")";
		return s;
	}
#endif  
};//~class Quaterion



//=================================
// inline functions implementation
//=================================

//TODO:



//=====================
// Convenient typedefs
//=====================

typedef Quaternion<float> Quatf;
STATIC_ASSERT(sizeof(Quatf) == sizeof(float) * 4)
typedef Quaternion<double> Quatd;
STATIC_ASSERT(sizeof(Quatd) == sizeof(double) * 4)



}//~namespace
