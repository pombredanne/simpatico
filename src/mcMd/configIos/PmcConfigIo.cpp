#ifndef PMC_CONFIG_IO_CPP
#define PMC_CONFIG_IO_CPP

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010, David Morse (morse@cems.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "PmcConfigIo.h"
#include <mcMd/simulation/System.h>
#include <mcMd/simulation/Simulation.h>
#include <mcMd/species/Species.h>
#include <mcMd/chemistry/Molecule.h>
#include <mcMd/chemistry/Atom.h>
#include <mcMd/boundary/OrthorhombicBoundary.h>
#include <util/space/Vector.h>
#include <util/param/Label.h>

#include <vector>

namespace McMd
{

   using namespace Util;

   /* 
   * Constructor.   
   */
   PmcConfigIo::PmcConfigIo(System &system) 
   : ConfigIo(system)
   {}
 
   /* 
   * Destructor.   
   */
   PmcConfigIo::~PmcConfigIo() 
   {}

   void PmcConfigIo::read(std::istream &in)
   {
      std::vector<int> nMoleculeSpecies;

      // Read and set dimensions of simulation Boundary.
      Vector lengths;
      in >> Label("BoxLengths") >> lengths;
      setLengths<Boundary>(boundary(), lengths);

      // Read total number of atoms (or particles)
      int nAtom, nSpecies;
      in >> Label("nParticle") >> nAtom ;
      in >> Label("nSpecies")  >> nSpecies ;
      if (nSpecies != simulation().nSpecies()) {
         UTIL_THROW("Input nSpecies != simulation().nSpecies()");
      }
      Log::file() << "nParticle =" << nAtom << std::endl;
      Log::file() << "nSpecies  =" << nSpecies << std::endl;


      // Read number of molecules of each species.
      Species  *speciesPtr;
      int       iSpec, nMolecule, nAtomSum;
      nAtomSum = 0;
      for (iSpec=0; iSpec < simulation().nSpecies(); ++iSpec) {
         speciesPtr = &simulation().species(iSpec);
         in >> Label("nMolecule") >> nMolecule;
         nMoleculeSpecies.push_back(nMolecule);
         nAtomSum += nMolecule*speciesPtr->nAtom();
      }
      if (nAtomSum != nAtom) {
         UTIL_THROW("Input nParticle != Atom Sum over Species");
      }
      Log::file() << "nAtomSum =" << nAtomSum;

      // Read atomic positions
      Molecule              *molPtr;
      Molecule::AtomIterator atomIter;
      int          atomType; // dummy, read and discarded
      for (iSpec=0; iSpec < simulation().nSpecies(); ++iSpec) {
         speciesPtr = &simulation().species(iSpec);
         nMolecule = nMoleculeSpecies[iSpec];
         for (int iMol=0; iMol < nMolecule; ++iMol) {
            molPtr = &(speciesPtr->reservoir().pop());
            system().addMolecule(*molPtr);
            for (molPtr->begin(atomIter); !atomIter.atEnd(); ++atomIter) {
               in >> atomIter->position() >> atomType;
               boundary().shift(atomIter->position());
            }
         }
      }

   }


   void PmcConfigIo::write(std::ostream &out)
   {
      using std::endl;

      // Write Boundary dimensions
      out << Label("lengths") << boundary().lengths() << endl;

      // Write number of molecules of each species
      int iSpec;
      for (iSpec=0; iSpec < simulation().nSpecies(); ++iSpec) {
         out << Label("nMolecule")
             << system().nMolecule(iSpec) << endl;
      }

      // Write atomic positions
      System::MoleculeIterator molIter;
      Molecule::AtomIterator   atomPtr;
      for (int iSpec=0; iSpec < simulation().nSpecies(); ++iSpec) {
         for (system().begin(iSpec, molIter); !molIter.atEnd(); ++molIter) {
            for (molIter->begin(atomPtr); !atomPtr.atEnd(); ++atomPtr) {
               out << atomPtr->position();
               out.width(10);
               out << atomPtr->typeId() << endl;
            }
         }
      }
   }

   template <class BoundaryType>
   void PmcConfigIo::setLengths(BoundaryType& boundary, const Vector& lengths)
   {  UTIL_THROW("Boundary type must be Orthorhombic for PmcConfigIo"); }
 
   template <>
   void PmcConfigIo::setLengths<OrthorhombicBoundary>(OrthorhombicBoundary& boundary, 
                                                      const Vector& lengths)
   {  boundary.setLengths(lengths);  }
 
} 
#endif