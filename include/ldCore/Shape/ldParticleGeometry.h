/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

//
//  ldParticleGeometry.h
//  Laserdock
//
//  Created by rock on 10/18/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#ifndef __Laserdock__ldParticleGeometry__
#define __Laserdock__ldParticleGeometry__

#include <ldCore/ldCore_global.h>

#include <iostream>
#include <float.h>
#include <stdint.h>
#include <stdlib.h>

typedef void            GLvoid;
typedef char            GLchar;
typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef float           GLfloat;
typedef float           GLclampf;
typedef int             GLfixed;
typedef int             GLclampx;
//typedef long            GLintptr;
//typedef long            GLsizeiptr;




class LDCORESHARED_EXPORT CCPoint
{
public:
    float x;
    float y;
    
public:
    CCPoint();
    CCPoint(float x, float y);
    CCPoint(const CCPoint& other);
    CCPoint& operator= (const CCPoint& other);
    void setPoint(float x, float y);
    bool equals(const CCPoint& target) const;
};

#define CCPointMake(x, y) CCPoint((float)(x), (float)(y))

#define ccp(__X__,__Y__) CCPointMake((float)(__X__), (float)(__Y__))


const CCPoint CCPointZero = CCPointMake(0,0);





class LDCORESHARED_EXPORT CCSize
{
public:
    float width;
    float height;
    
public:
    CCSize();
    CCSize(float width, float height);
    CCSize(const CCSize& other);
    CCSize& operator= (const CCSize& other);
    void setSize(float width, float height);
    bool equals(const CCSize& target) const;
};


#define CCSizeMake(width, height) CCSize((float)(width), (float)(height))

/** RGB color composed of bytes 3 bytes
 @since v0.8
 */
typedef struct _ccColor3B
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
} ccColor3B;

//! helper macro that creates an ccColor3B type
static inline ccColor3B
ccc3(const GLubyte r, const GLubyte g, const GLubyte b)
{
    ccColor3B c = {r, g, b};
    return c;
}
//ccColor3B predefined colors
//! White color (255,255,255)
static const ccColor3B ccWHITE={255,255,255};
//! Yellow color (255,255,0)
static const ccColor3B ccYELLOW={255,255,0};
//! Blue color (0,0,255)
static const ccColor3B ccBLUE={0,0,255};
//! Green Color (0,255,0)
static const ccColor3B ccGREEN={0,255,0};
//! Red Color (255,0,0,)
static const ccColor3B ccRED={255,0,0};
//! Magenta Color (255,0,255)
static const ccColor3B ccMAGENTA={255,0,255};
//! Black Color (0,0,0)
static const ccColor3B ccBLACK={0,0,0};
//! Orange Color (255,127,0)
static const ccColor3B ccORANGE={255,127,0};
//! Gray Color (166,166,166)
static const ccColor3B ccGRAY={166,166,166};

/** RGBA color composed of 4 bytes
 @since v0.8
 */
typedef struct _ccColor4B
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} ccColor4B;
//! helper macro that creates an ccColor4B type
static inline ccColor4B
ccc4(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte o)
{
    ccColor4B c = {r, g, b, o};
    return c;
}


/** RGBA color composed of 4 floats
 @since v0.8
 */
typedef struct _ccColor4F {
//    _ccColor4F() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} ccColor4F;


/** Returns a ccColor4F from a ccColor3B. Alpha will be 1.
 @since v0.99.1
 */
static inline ccColor4F ccc4FFromccc3B(ccColor3B c)
{
    ccColor4F c4 = {c.r/255.f, c.g/255.f, c.b/255.f, 1.f};
    return c4;
}

//! helper that creates a ccColor4f type
static inline ccColor4F
ccc4f(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a)
{
    ccColor4F c4 = {r, g, b, a};
    return c4;
}

/** Returns a ccColor4F from a ccColor4B.
 @since v0.99.1
 */
static inline ccColor4F ccc4FFromccc4B(ccColor4B c)
{
    ccColor4F c4 = {c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f};
    return c4;
}

static inline ccColor4B ccc4BFromccc4F(ccColor4F c)
{
    ccColor4B ret = {(GLubyte)(c.r*255), (GLubyte)(c.g*255), (GLubyte)(c.b*255), (GLubyte)(c.a*255)};
	return ret;
}

/** returns YES if both ccColor4F are equal. Otherwise it returns NO.
 @since v0.99.1
 */
static inline bool ccc4FEqual(ccColor4F a, ccColor4F b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}







/** CC_PROPERTY_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public virtual function, you should rewrite it first.
 The variables and methods declared after CC_PROPERTY_READONLY are all public.
 If you need protected or private, please declare.
 */
#define CC_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

#define CC_PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);

/** CC_PROPERTY is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public virtual functions, you should rewrite them first.
 The variables and methods declared after CC_PROPERTY are all public.
 If you need protected or private, please declare.
 */
#define CC_PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

#define CC_PROPERTY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);\
public: virtual void set##funName(const varType& var);

/** CC_SYNTHESIZE_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public inline function.
 The variables and methods declared after CC_SYNTHESIZE_READONLY are all public.
 If you need protected or private, please declare.
 */
#define CC_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define CC_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }

/** CC_SYNTHESIZE is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public  inline functions.
 The variables and methods declared after CC_SYNTHESIZE are all public.
 If you need protected or private, please declare.
 */
#define CC_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

#define CC_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }

#define CC_SYNTHESIZE_RETAIN(varType, varName, funName)    \
private: varType varName; \
public: virtual varType get##funName(void) const { return varName; } \
public: virtual void set##funName(varType var)   \
{ \
if (varName != var) \
{ \
CC_SAFE_RETAIN(var); \
CC_SAFE_RELEASE(varName); \
varName = var; \
} \
}

#define CC_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)





/** @def CCRANDOM_MINUS1_1
 returns a random float between -1 and 1
 */
#define CCRANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

/** @def CCRANDOM_0_1
 returns a random float between 0 and 1
 */
#define CCRANDOM_0_1() ((float)rand()/RAND_MAX)


#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX


/** @def CC_SWAP
 simple macro that swaps 2 variables
 */
#define CC_SWAP(x, y, type)    \
{    type temp = (x);        \
x = y; y = temp;        \
}


/** @def CC_DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define CC_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def CC_RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define CC_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180






#define kCCPointEpsilon FLT_EPSILON

/** Returns opposite of point.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpNeg(const CCPoint& v)
{
    return ccp(-v.x, -v.y);
}

/** Calculates sum of two points.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpAdd(const CCPoint& v1, const CCPoint& v2)
{
    return ccp(v1.x + v2.x, v1.y + v2.y);
}

/** Calculates difference of two points.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpSub(const CCPoint& v1, const CCPoint& v2)
{
    return ccp(v1.x - v2.x, v1.y - v2.y);
}

/** Returns point multiplied by given factor.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpMult(const CCPoint& v, const float s)
{
    return ccp(v.x*s, v.y*s);
}

/** Calculates midpoint between two points.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpMidpoint(const CCPoint& v1, const CCPoint& v2)
{
    return ccpMult(ccpAdd(v1, v2), 0.5f);
}

/** Calculates dot product of two points.
 @return float
 @since v0.7.2
 */
static inline float
ccpDot(const CCPoint& v1, const CCPoint& v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

/** Calculates cross product of two points.
 @return float
 @since v0.7.2
 */
static inline float
ccpCross(const CCPoint& v1, const CCPoint& v2)
{
    return v1.x*v2.y - v1.y*v2.x;
}

/** Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpPerp(const CCPoint& v)
{
    return ccp(-v.y, v.x);
}

/** Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpRPerp(const CCPoint& v)
{
    return ccp(v.y, -v.x);
}

/** Calculates the projection of v1 over v2.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpProject(const CCPoint& v1, const CCPoint& v2)
{
    return ccpMult(v2, ccpDot(v1, v2)/ccpDot(v2, v2));
}

/** Rotates two points.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpRotate(const CCPoint& v1, const CCPoint& v2)
{
    return ccp(v1.x*v2.x - v1.y*v2.y, v1.x*v2.y + v1.y*v2.x);
}

/** Unrotates two points.
 @return CCPoint
 @since v0.7.2
 */
static inline CCPoint
ccpUnrotate(const CCPoint& v1, const CCPoint& v2)
{
    return ccp(v1.x*v2.x + v1.y*v2.y, v1.y*v2.x - v1.x*v2.y);
}

/** Calculates the square length of a CCPoint (not calling sqrt() )
 @return float
 @since v0.7.2
 */
static inline float
ccpLengthSQ(const CCPoint& v)
{
    return ccpDot(v, v);
}


/** Calculates the square distance between two points (not calling sqrt() )
 @return float
 @since v1.1
 */
static inline float
ccpDistanceSQ(const CCPoint p1, const CCPoint p2)
{
    return ccpLengthSQ(ccpSub(p1, p2));
}

/** Calculates distance between point an origin
 @return float
 @since v0.7.2
 */
LDCORESHARED_EXPORT float ccpLength(const CCPoint& v);

/** Calculates the distance between two points
 @return float
 @since v0.7.2
 */
LDCORESHARED_EXPORT float ccpDistance(const CCPoint& v1, const CCPoint& v2);

/** Returns point multiplied to a length of 1.
 @return CCPoint
 @since v0.7.2
 */
LDCORESHARED_EXPORT CCPoint ccpNormalize(const CCPoint& v);

/** Converts radians to a normalized vector.
 @return CCPoint
 @since v0.7.2
 */
LDCORESHARED_EXPORT CCPoint ccpForAngle(const float a);

/** Converts a vector to radians.
 @return float
 @since v0.7.2
 */
LDCORESHARED_EXPORT float ccpToAngle(const CCPoint& v);


/** Clamp a value between from and to.
 @since v0.99.1
 */
LDCORESHARED_EXPORT float clampf(float value, float min_inclusive, float max_inclusive);

/** Clamp a point between from and to.
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpClamp(const CCPoint& p, const CCPoint& from, const CCPoint& to);

/** Quickly convert CCSize to a CCPoint
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpFromSize(const CCSize& s);

/** Run a math operation function on each point component
 * absf, fllorf, ceilf, roundf
 * any function that has the signature: float func(float);
 * For example: let's try to take the floor of x,y
 * ccpCompOp(p,floorf);
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpCompOp(const CCPoint& p, float (*opFunc)(float));

/** Linear Interpolation between two points a and b
 @returns
 alpha == 0 ? a
 alpha == 1 ? b
 otherwise a value between a..b
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpLerp(const CCPoint& a, const CCPoint& b, float alpha);


/** @returns if points have fuzzy equality which means equal with some degree of variance.
 @since v0.99.1
 */
LDCORESHARED_EXPORT bool ccpFuzzyEqual(const CCPoint& a, const CCPoint& b, float variance);


/** Multiplies a and b components, a.x*b.x, a.y*b.y
 @returns a component-wise multiplication
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpCompMult(const CCPoint& a, const CCPoint& b);

/** @returns the signed angle in radians between two vector directions
 @since v0.99.1
 */
LDCORESHARED_EXPORT float ccpAngleSigned(const CCPoint& a, const CCPoint& b);

/** @returns the angle in radians between two vector directions
 @since v0.99.1
 */
LDCORESHARED_EXPORT float ccpAngle(const CCPoint& a, const CCPoint& b);

/** Rotates a point counter clockwise by the angle around a pivot
 @param v is the point to rotate
 @param pivot is the pivot, naturally
 @param angle is the angle of rotation cw in radians
 @returns the rotated point
 @since v0.99.1
 */
LDCORESHARED_EXPORT CCPoint ccpRotateByAngle(const CCPoint& v, const CCPoint& pivot, float angle);

/** A general line-line intersection test
 @param p1
 is the startpoint for the first line P1 = (p1 - p2)
 @param p2
 is the endpoint for the first line P1 = (p1 - p2)
 @param p3
 is the startpoint for the second line P2 = (p3 - p4)
 @param p4
 is the endpoint for the second line P2 = (p3 - p4)
 @param s
 is the range for a hitpoint in P1 (pa = p1 + s*(p2 - p1))
 @param t
 is the range for a hitpoint in P3 (pa = p2 + t*(p4 - p3))
 @return bool
 indicating successful intersection of a line
 note that to truly test intersection for segments we have to make
 sure that s & t lie within [0..1] and for rays, make sure s & t > 0
 the hit point is        p3 + t * (p4 - p3);
 the hit point also is    p1 + s * (p2 - p1);
 @since v0.99.1
 */
LDCORESHARED_EXPORT bool ccpLineIntersect(const CCPoint& p1, const CCPoint& p2,
                             const CCPoint& p3, const CCPoint& p4,
                             float *s, float *t);

/*
 ccpSegmentIntersect returns YES if Segment A-B intersects with segment C-D
 @since v1.0.0
 */
LDCORESHARED_EXPORT bool ccpSegmentIntersect(const CCPoint& A, const CCPoint& B, const CCPoint& C, const CCPoint& D);

/*
 ccpIntersectPoint returns the intersection point of line A-B, C-D
 @since v1.0.0
 */
LDCORESHARED_EXPORT CCPoint ccpIntersectPoint(const CCPoint& A, const CCPoint& B, const CCPoint& C, const CCPoint& D);




#endif /* defined(__Laserdock__ldParticleGeometry__) */
