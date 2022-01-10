//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "spline.h"

using namespace std;


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CSPLine::CSPLine()
{
  m_fContinuity = 0.0f;
  m_fBias = 0.0f;
  m_fTension = 0.0f;
  m_Keys.clear();
}



// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CSPLine::SetContinuity(float fContinuity)
{
  if (fContinuity < -1.0f) fContinuity = -1.0f;
  if (fContinuity >  1.0f) fContinuity =  1.0f;
  m_fContinuity = fContinuity;
}



// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CSPLine::SetBias(float fBias)
{
  if (fBias < -1.0f) fBias = -1.0f;
  if (fBias >  1.0f) fBias =  1.0f;
  m_fBias = fBias;
}



// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CSPLine::SetTension(float fTension)
{
  if (fTension < -1.0f) fTension = -1.0f;
  if (fTension >  1.0f) fTension =  1.0f;
  m_fTension = fTension;
}


void CSPLine::AddKey(unique_ptr<ldVec2> pVector)
{
    m_Keys.push_back(move(pVector));
}


// ------------------------------------------------------------------------------------------------
// Interpolate the spline and return the projected CVector.
//
// fFrame is the time between keyframes, like:
// 0.0 .. 1.0 is the time between the first and second keyframes,
// 1.0 .. 2.0 is the time between the second and third keyframes,
// and so on.
// ------------------------------------------------------------------------------------------------
ldVec2 CSPLine::Point(float fFrame)
{
  int i_1, i0, i1, i2;
  float time1,time2,time3;
  float t1,t2,t3,t4,u1,u2,u3,u4,v1,v2,v3;
  float a,b,c,d;

  int iNumKeys = m_Keys.size();
  if (iNumKeys == 0) return ldVec2(0,0);

  a = (1-m_fTension) * (1+m_fContinuity) * (1+m_fBias);
  b = (1-m_fTension) * (1-m_fContinuity) * (1-m_fBias);
  c = (1-m_fTension) * (1-m_fContinuity) * (1+m_fBias);
  d = (1-m_fTension) * (1+m_fContinuity) * (1-m_fBias);

  v1 = t1 = -a / 2.0f;
  u1 = a;

  u2 = (-6-2*a+2*b+c)/2.0f;
  v2 = (a-b)/2.0f;
  t2 = (4+a-b-c) / 2.0f;

  t3 = (-4+b+c-d) / 2.0f;
  u3 = (6-2*b-c+d) / 2.0f;
  v3 = b/2.0f;
  t4 = d/2.0f;
  u4 = -t4;

  i0 = (uint) fFrame;        // determine the exact keyframe
  i_1 = i0 - 1;               // determine the previous keyframe
  while (i_1 < 0)             // if the previous is at the end,
    i_1 += iNumKeys;          // wrap around

  i1 = i0 + 1;                // determine the next keyframe
  while (i1 >= iNumKeys)      // if the next is at the start
    i1 -= iNumKeys;           // wrap around

  i2 = i0 + 2;                // determine the next next keyframe
  while (i2 >= iNumKeys)      // if the next next is at the start
    i2 -= iNumKeys;           // wrap around

  time1 = fFrame - (float)((uint) fFrame);
  time2 = time1*time1;
  time3 = time2*time1;

  //ldVec2* k0 = m_Keys[i0];
  //ldVec2* k1 = m_Keys[i1];
  //ldVec2* k2 = m_Keys[i2];
  //ldVec2* k_1 = m_Keys[i_1];

  ldVec2 v;    // resultant

  a = t1 * m_Keys[i_1]->x + t2 * m_Keys[i0]->x + t3 * m_Keys[i1]->x + t4 * m_Keys[i2]->x;
  b = u1 * m_Keys[i_1]->x + u2 * m_Keys[i0]->x + u3 * m_Keys[i1]->x + u4 * m_Keys[i2]->x;
  c = v1 * m_Keys[i_1]->x + v2 * m_Keys[i0]->x + v3 * m_Keys[i1]->x;
  v.x = a*time3 + b*time2 + c*time1 + m_Keys[i0]->x;

  a = t1 * m_Keys[i_1]->y + t2 * m_Keys[i0]->y + t3 * m_Keys[i1]->y + t4 * m_Keys[i2]->y;
  b = u1 * m_Keys[i_1]->y + u2 * m_Keys[i0]->y + u3 * m_Keys[i1]->y + u4 * m_Keys[i2]->y;
  c = v1 * m_Keys[i_1]->y + v2 * m_Keys[i0]->y + v3 * m_Keys[i1]->y;
  v.y = a*time3 + b*time2 + c*time1 + m_Keys[i0]->y;

  return v;
}
