#ifndef INTER_ANGLE_H
#define INTER_ANGLE_H

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010 - 2012, Jian Qin and David Morse (morse012@umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/space/Vector.h>

#include <cmath>

namespace Inter
{

   using namespace Util;

   /**
   * An angle between two vectors.
   *
   * The angle between a pair of vectors b1 and b2, which must be
   * passed to the functions computeAngle() or computeDerivatives().
   *
   * The scalar cosTheta is the cosine of the angle between b1 and b2.
   *
   * The vectors u1 and u2 are unit vectors parallel to b1 and b2.
   * 
   * The vectors d1 and d2 (if calculated) are vectors whose elements
   * are the derivatives of cosTheta with respect to the elements of
   * b1 and b2, respectively.
   * 
   * \ingroup Inter_Angle_Module
   */
   struct Angle 
   {
   
   public:

      /**
      * Cosine of angle between vectors b1 and b2.
      */
      double cosTheta;

      /**
      * Unit vector parallel to b1.
      */
      Vector u1;

      /**
      * Unit vector parallel to b2.
      */
      Vector u2;

      /**
      * Vector of derivatives d1[i] = d(cosTheta)/d(b1[i])
      */
      Vector d1;

      /**
      * Vector of derivatives d2[i] = d(cosTheta)/d(b2[i])
      */
      Vector d2;
   
      /**
      * Compute unit vectors u1 and u2 and cosTheta.
      *
      * \param b1  bond vector from atom 0 to 1.
      * \param b2  bond vector from atom 1 to 2.
      */
      void computeAngle(const Vector& b1, const Vector& b2);
 
      /**
      * Compute unit vectors, cosTheta, and derivatives d1 and d2.
      *
      * \param b1  bond vector from atom 0 to 1.
      * \param b2  bond vector from atom 1 to 2.
      */
      void computeDerivatives(const Vector& b1, const Vector& b2);

      /**
      * Return value of sin(theta) for precomputed cos(theta).
      */
      double sinTheta() const;

      /**
      * Return value of theta in radians for precomputed cos(theta).
      *
      * Returns principal value, in range 0 < theta < Pi.
      */
      double theta() const;

   };

   // Inline method definitions
   
   /* 
   * Calculate unit bond vectors and cosTheta.
   */
   inline
   void Angle::computeAngle(const Vector& b1, const Vector& b2) 
   {
      double b1Abs = b1.abs();
      u1.divide(b1, b1Abs);

      double b2Abs = b2.abs();
      u2.divide(b2, b2Abs);

      cosTheta = u1.dot(u2);

      if (cosTheta >  1.0) cosTheta =  1.0;
      if (cosTheta < -1.0) cosTheta = -1.0;
   }

   /* 
   * Calculate cosTheta and unit vectors.
   */
   inline
   void Angle::computeDerivatives(const Vector& b1, const Vector& b2)
   {
      double b1Abs = b1.abs();
      u1.divide(b1, b1Abs);

      double b2Abs = b2.abs();
      u2.divide(b2, b2Abs);

      cosTheta = u1.dot(u2);

      Vector t;
      t.multiply(u1, cosTheta);
      d1.subtract(u2, t);
      d1 /= b1Abs;

      t.multiply(u2, cosTheta);
      d2.subtract(u1, t);
      d2 /= b2Abs;
   }

   /*
   * Return value sin(theta) for precomputed cos(theta).
   */
   inline
   double Angle::sinTheta() const
   { return sqrt(1.0 - cosTheta*cosTheta); }

   /*
   * Return theta in radians for precomputed cos(theta).
   */
   inline
   double Angle::theta() const
   {  return std::acos(cosTheta); }

} 
#endif