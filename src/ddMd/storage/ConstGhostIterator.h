#ifndef DDMD_CONST_GHOST_ITERATOR_H
#define DDMD_CONST_GHOST_ITERATOR_H

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010 - 2012, David Morse (morse012@umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include <util/containers/ConstPArrayIterator.h>

namespace DdMd
{

   using namespace Util;
   class Atom;

   /**
   * Iterator for all ghost atoms owned by an AtomStorage.
   *
   * \ingroup DdMd_Storage_Module
   */
   class ConstGhostIterator : public ConstPArrayIterator<Atom>
   {};

}
#endif
