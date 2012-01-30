#ifdef  MCMD_EXTERNAL
#ifndef EXTERNAL_FACTORY_H
#define EXTERNAL_FACTORY_H

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010, David Morse (morse@cems.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/param/Factory.h>
#include <iostream>

namespace McMd
{

   using namespace Util;

   class System;
   class ExternalPotential;

   /**
   * Factory for subclasses MdExternalPotential or McExternalPotential.
   * 
   * \ingroup External_Module
   */
   class ExternalFactory : public Factory<ExternalPotential>
   {
   
   public:
   
      /**
      * Default constructor.
      */
      ExternalFactory(System& system);

      /**
      * Return a pointer to a new ExternalInteration, if possible.
      */
      ExternalPotential* factory(const std::string& subclass) const;

   private:

      // Pointer to parent system.
      System* systemPtr_;

   };
  
}
#endif
#endif