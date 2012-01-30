#ifndef MD_DIAGNOSTIC_MANAGER_CPP
#define MD_DIAGNOSTIC_MANAGER_CPP

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010, David Morse (morse@cems.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "MdDiagnosticManager.h"                
#include "MdSimulation.h"  
#include <mcMd/diagnostics/mdSystem/MdDiagnosticFactory.h> 

namespace McMd
{

   using namespace Util;

   // Constructor.
   MdDiagnosticManager::MdDiagnosticManager(MdSimulation& simulation)
    : simulationPtr_(&simulation),
      systemPtr_(&simulation.system())
   {}

   // Constructor.
   MdDiagnosticManager::MdDiagnosticManager(MdSimulation& simulation, 
		                            MdSystem& system)
    : simulationPtr_(&simulation),
      systemPtr_(&system)
   {}

   // Destructor.
   MdDiagnosticManager::~MdDiagnosticManager()
   {}

   /// Return pointer to a new DiagnosticFactory.
   Factory<Diagnostic>* MdDiagnosticManager::newDefaultFactory() const
   {  return new MdDiagnosticFactory(*simulationPtr_, *systemPtr_); }

}
#endif