#ifndef DDMD_DIAGNOSTIC_FACTORY_H
#define DDMD_DIAGNOSTIC_FACTORY_H

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010 - 2012, David Morse (morse012@umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/param/Factory.h>            // base class template
#include <ddMd/diagnostics/Diagnostic.h>   // base template parameter

namespace DdMd
{

   using namespace Util;

   class Simulation;

   /**
   * Factory for DdMd::Diagnostic objects.
   *
   * \ingroup DdMd_Factory_Module
   * \ingroup DdMd_Diagnostic_Module
   */
   class DiagnosticFactory : public Factory<Diagnostic>
   {

   public:

      /**
      * Constructor.
      *
      * \param simulation     parent Simulation
      */
      DiagnosticFactory(Simulation& simulation);

      /** 
      * Return pointer to a new Diagnostic object.
      *
      * \param  className name of a subclass of Diagnostic.
      * \return base class pointer to a new instance of className.
      */
      virtual Diagnostic* factory(const std::string& className) const;

   protected:

      /**
      * Return reference to parent Simulation.
      */
      Simulation& simulation() const
      {  return *simulationPtr_; }

   private:

      /// Pointer to parent Simulation.
      Simulation* simulationPtr_;

   };

}
#endif
