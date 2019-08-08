
#pragma once
#include <cmath>

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

// Math stuffs
#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define RADPI 57.295779513082f

class Vector {
public:
	float x, y, z;
	Vector( float X = 0.0f, float Y = 0.0f, float Z = 0.0f );
	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );
	float operator[]( int i ) const;
	float& operator[]( int i );
	inline void Zero();
	bool operator==( const Vector& src ) const;
	bool operator!=( const Vector& src ) const;
	__forceinline Vector& operator+=( const Vector& v );
	__forceinline Vector& operator-=( const Vector& v );
	__forceinline Vector& operator*=( const Vector& v );
	__forceinline Vector& operator*=( float fl );
	__forceinline Vector& operator/=( const Vector& v );
	__forceinline Vector& operator/=( float fl );
	__forceinline Vector& operator+=( float fl );
	__forceinline Vector& operator-=( float fl );
	inline float  Length() const;
	__forceinline float LengthSqr() const {
		CHECK_VALID( *this );
		return ( x * x + y * y + z * z );
	}
	bool IsZero( float tolerance = 0.01f ) const {
		return ( x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance );
	}
	float Length2d() const {
		return sqrtf( x * x + y * y );
	}
	float DistTo( const Vector& v ) const {
		return ( *this - v ).Length();
	}
	void Set( float X = 0.0f, float Y = 0.0f, float Z = 0.0f ) {
		x = X;
		y = Y;
		z = Z;
	}
	void NormalizeInPlace();
	__forceinline float DistToSqr( const Vector& vOther ) const;
	float Dot( const Vector& vOther ) const;
	float Length2D() const;
	float Length2DSqr() const;
	Vector& operator=( const Vector& vOther );
	Vector  operator-() const;
	Vector  operator+( const Vector& v ) const;
	Vector  operator-( const Vector& v ) const;
	Vector  operator*( const Vector& v ) const;
	Vector  operator/( const Vector& v ) const;
	Vector  operator*( float fl ) const;
	Vector  operator/( float fl ) const;
};

inline void fClampAngle( Vector& qaAng ) {
	while( qaAng[0] > 89 ) {
		qaAng[0] -= 180;
	}

	while( qaAng[0] < -89 ) {
		qaAng[0] += 180;
	}

	while( qaAng[1] > 180 ) {
		qaAng[1] -= 360;
	}

	while( qaAng[1] < -180 ) {
		qaAng[1] += 360;
	}

	qaAng.z = 0;
}
//===============================================
inline void Vector::Init( float ix, float iy, float iz ) {
	x = ix;
	y = iy;
	z = iz;
	CHECK_VALID( *this );
}
//===============================================
inline Vector::Vector( float X, float Y, float Z ) {
	x = X;
	y = Y;
	z = Z;
	CHECK_VALID( *this );
}

//===============================================
inline void Vector::Zero() {
	x = y = z = 0.0f;
}
//===============================================
inline void VectorClear( Vector& a ) {
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector& Vector::operator=( const Vector& vOther ) {
	CHECK_VALID( vOther );
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	return *this;
}
//===============================================
inline float& Vector::operator[]( int i ) {
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( (float*)this )[i];
}
//===============================================
inline float Vector::operator[]( int i ) const {
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( (float*)this )[i];
}
//===============================================
inline bool Vector::operator==( const Vector& src ) const {
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x == x ) && ( src.y == y ) && ( src.z == z );
}
//===============================================
inline bool Vector::operator!=( const Vector& src ) const {
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x != x ) || ( src.y != y ) || ( src.z != z );
}
//===============================================
__forceinline void VectorCopy( const Vector& src, Vector& dst ) {
	CHECK_VALID( src );
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
__forceinline  Vector& Vector::operator+=( const Vector& v ) {
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator-=( const Vector& v ) {
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator*=( float fl ) {
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator*=( const Vector& v ) {
	CHECK_VALID( v );
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
__forceinline Vector& Vector::operator+=( float fl ) {
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
__forceinline Vector& Vector::operator-=( float fl ) {
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator/=( float fl ) {
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator/=( const Vector& v ) {
	CHECK_VALID( v );
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline float Vector::Length() const {
	CHECK_VALID( *this );
	return sqrt( x * x + y * y + z * z );
}
//===============================================
inline float Vector::Length2D() const {
	CHECK_VALID( *this );
	return sqrt( x * x + y * y );
}
//===============================================
inline float Vector::Length2DSqr() const {
	return ( x * x + y * y );
}
//===============================================
inline Vector CrossProduct( const Vector& a, const Vector& b ) {
	return Vector( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}
//===============================================
float Vector::DistToSqr( const Vector& vOther ) const {
	Vector delta;
	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;
	return delta.LengthSqr();
}
//===============================================
inline void Vector::NormalizeInPlace() {
	Vector& v = *this;
	float iradius = 1.f / ( this->Length() + 1.192092896e-07F ); //FLT_EPSILON
	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
}
//===============================================
inline Vector Vector::operator+( const Vector& v ) const {
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}
//===============================================
inline Vector Vector::operator-( const Vector& v ) const {
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}
//===============================================
inline Vector Vector::operator*( float fl ) const {
	Vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}
//===============================================
inline Vector Vector::operator*( const Vector& v ) const {
	Vector res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}
//===============================================
inline Vector Vector::operator/( float fl ) const {
	Vector res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}
//===============================================
inline Vector Vector::operator/( const Vector& v ) const {
	Vector res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float Vector::Dot( const Vector& vOther ) const {
	const Vector& a = *this;
	return( a.x * vOther.x + a.y * vOther.y + a.z * vOther.z );
}
inline void SinCos( float radians, float* sine, float* cosine ) {
	*sine = sin( radians );
	*cosine = cos( radians );
}

inline void AngleVectors( const Vector& angles, Vector* forward, Vector* right, Vector* up ) {
	float sr, sp, sy, cr, cp, cy;
	SinCos( DEG2RAD( angles.x ), &sp, &cp );
	SinCos( DEG2RAD( angles.y ), &sy, &cy );
	SinCos( DEG2RAD( angles.z ), &sr, &cr );

	if( forward ) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if( right ) {
		right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
		right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
		right->z = -1 * sr * cp;
	}

	if( up ) {
		up->x = ( cr * sp * cy + -sr * -sy );
		up->y = ( cr * sp * sy + -sr * cy );
		up->z = cr * cp;
	}
}
inline void VectorAngles( Vector &forward, Vector& angles ) {
	float yaw, pitch;

	if( forward.y == 0 && forward.x == 0 ) {
		yaw = 0;

		if( forward.z > 0 ) {
			pitch = 270;
		} else {
			pitch = 90;
		}
	} else {
		yaw = RAD2DEG( atan2f( forward.y, forward.x ) );

		if( yaw < 0 ) {
			yaw += 360;
		}

		float tmp = forward.Length2D();
		pitch = RAD2DEG( atan2f( -forward.z, tmp ) );

		if( pitch < 0 ) {
			pitch += 360;
		}
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

inline void ClampAngle( Vector& qaAng ) {
	while( qaAng.x > 89 ) {
		qaAng.x -= 180;
	}

	while( qaAng.x < -89 ) {
		qaAng.x += 180;
	}

	while( qaAng.y > 180 ) {
		qaAng.y -= 360;
	}

	while( qaAng.y < -180 ) {
		qaAng.y += 360;
	}

	while( qaAng.z != 0 ) {
		qaAng.z = 0;
	}
}

inline Vector AngleVector( const Vector& angles ) {
	float sy = sinf( DEG2RAD( angles[1] ) ); // yaw
	float cy = cosf( DEG2RAD( angles[1] ) );
	float sp = sinf( DEG2RAD( angles[0] ) ); // pitch
	float cp = cosf( DEG2RAD( angles[0] ) );
	return { cp * cy,cp * sy,-sp };
}

inline void AngleNormalize( Vector& v ) {
	for( int i = 0; i < 3; i++ ) {
		if( v[i] < -180.0f ) {
			v[i] += 360.0f;
		}

		if( v[i] > 180.0f ) {
			v[i] -= 360.0f;
		}
	}
}

inline float Dot( const Vector& a, const Vector& b ) {
	CHECK_VALID( a );
	CHECK_VALID( b );
	return( a.x * b.x + a.y * b.y + a.z * b.z );
}