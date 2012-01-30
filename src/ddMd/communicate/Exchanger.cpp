#ifndef EXCHANGER_CPP
#define EXCHANGER_CPP

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010, David Morse (morse@cems.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "Exchanger.h"
#include "Domain.h"
#include "Buffer.h"
#include <ddMd/storage/AtomStorage.h>
#include <ddMd/storage/AtomIterator.h>
#include <ddMd/storage/GhostIterator.h>
#include <util/mpi/MpiLogger.h>
#include <util/global.h>

#include <algorithm>
#include <string>

namespace DdMd
{
   using namespace Util;

   /*
   * Constructor.
   */
   Exchanger::Exchanger()
    : pairCutoff_(-1.0),
      ghostCapacity_(0)
   {}

   /*
   * Destructor.
   */
   Exchanger::~Exchanger()
   {}

   /*
   * Set pointers to associated objects.
   */
   void Exchanger::associate(const Boundary& boundary, const Domain& domain, 
                             AtomStorage& storage, Buffer& buffer)
   {
      boundaryPtr_  = &boundary;
      domainPtr_    = &domain;
      storagePtr_   = &storage;
      bufferPtr_    = &buffer;
   }

   /*
   * Allocate memory.
   */
   void Exchanger::allocate()
   {
      // Preconditions
      if (!bufferPtr_->isInitialized()) {
         UTIL_THROW("Buffer must be allocated before Exchanger");
      }

      // Allocate all send and recv arrays
      ghostCapacity_ = std::max(bufferPtr_->atomCapacity(), bufferPtr_->ghostCapacity());
      int i, j;
      for (i = 0; i < Dimension; ++i) {
         for (j = 0; j < 2; ++j) {
            sendArray_(i, j).allocate(ghostCapacity_);
            recvArray_(i, j).allocate(ghostCapacity_);
         }
      }

   }

   /*
   * Set slab width used for ghosts.
   */
   void Exchanger::setPairCutoff(double pairCutoff)
   {  pairCutoff_ = pairCutoff; }

   #ifdef UTIL_MPI

   /**
   * Exchange ownership of local atoms.
   *
   * This method should be called before rebuilding the neighbor list on
   * each processor, to exchange ownership of local atoms.
   */
   void Exchanger::exchangeAtoms()
   {
      Vector        lengths = boundaryPtr_->lengths();
      double        bound;
      AtomIterator  iter;
      Atom*         ptr;
      int           i, j, k, source, dest, nSend;
      int           shift;
      bool          choose;

      // Cartesian directions
      for (i = 0; i < Dimension; ++i) {

         // Transmission direction
         for (j = 0; j < 2; ++j) {

            // j = 0 sends to lower  coordinate i
            // j = 1 sends to higher coordinate i

            //Processor to receive from
            source = domainPtr_->sourceRank(i, j);

            //Processor to send to
            dest   = domainPtr_->destRank(i, j);

            // Boundary (j=0 -> minimum, j=1 -> maximum)
            bound = domainPtr_->domainBound(i, j);

            // Shift due to periodic boundary conditions
            shift = domainPtr_->shift(i, j);

            sendArray_(i, j).clear();

            if(domainPtr_->grid().dimension(i) > 1) {
               bufferPtr_->clearSendBuffer();
               bufferPtr_->beginSendBlock(Buffer::ATOM);
            }

            // Choose atoms for sending, pack and mark for removal.
            storagePtr_->begin(iter);
            for ( ; !iter.atEnd(); ++iter) {

               if (j == 0) {
                  choose = (iter->position()[i] < bound);
               } else {
                  choose = (iter->position()[i] > bound);
               }

               if (choose) {

                  if (domainPtr_->grid().dimension(i) > 1) {

                     sendArray_(i, j).append(*iter);
                     bufferPtr_->packAtom(*iter);

                  } else {

                     // Shift position if required by periodic b.c.
                     if (shift) {
                        iter->position()[i] += shift * lengths[i];
                     }
                     assert(iter->position()[i] > domainPtr_->domainBound(i, 0));
                     assert(iter->position()[i] < domainPtr_->domainBound(i, 1));

                  }
               }

            } // end atom loop

            // Send and receive only if dimension(i) > 1
            if(domainPtr_->grid().dimension(i) > 1) {

               bufferPtr_->endSendBlock();

               // Remove chosen atoms from storage
               // This cannot be done within the above loop over atoms because
               // removal invalidates the AtomIterator.
               nSend = sendArray_(i, j).size();
               for (k = 0; k < nSend; ++k) {
                  storagePtr_->removeAtom(&sendArray_(i, j)[k]);
               }

               // Send to processor dest and receive from processor source
               bufferPtr_->sendRecv(domainPtr_->communicator(), source, dest);

               // Unpack atoms from receive buffer into storage
               bufferPtr_->beginRecvBlock();
               while (bufferPtr_->recvSize() > 0) {
                  ptr = storagePtr_->newAtomPtr();
                  bufferPtr_->unpackAtom(*ptr);
                  if (shift) {
                     ptr->position()[i] += shift * lengths[i];
                  }
                  assert(ptr->position()[i] > domainPtr_->domainBound(i, 0));
                  assert(ptr->position()[i] < domainPtr_->domainBound(i, 1));
                  storagePtr_->addNewAtom();
               }

            }
         }
      }
   }

   /*
   * Exchange ghost atoms.
   *
   * Call after exchangeAtoms and before rebuilding the neighbor list on
   * time steps that require reneighboring.
   */
   void Exchanger::exchangeGhosts()
   {

      // Preconditions
      assert(bufferPtr_->isInitialized());
      assert(domainPtr_->isInitialized());
      assert(domainPtr_->hasBoundary());

      Vector        lengths = boundaryPtr_->lengths();
      double        bound, inner, coord;
      AtomIterator  localIter;
      GhostIterator ghostIter;
      Atom*         ptr;
      int           i, j, source, dest;
      bool          choose;
      int           shift;

      // Check that all ghosts are cleared upon entry.
      if (storagePtr_->nGhost() > 0) {
         UTIL_THROW("storagePtr_->nGhost() > 0");
      }

      // Cartesian directions
      for (i = 0; i < Dimension; ++i) {

         // Transmit directions
         for (j = 0; j < 2; ++j) {

            // j = 0: Send ghosts near minimum boundary to lower coordinate
            // j = 1: Send ghosts near maximum boundary to higher coordinate

            if (j == 0) {
               bound = domainPtr_->domainBound(i, 0);
               inner = bound + pairCutoff_;
            } else {
               bound = domainPtr_->domainBound(i, 1);
               inner = bound - pairCutoff_;
            }

            // Shift on receiving processor for periodic boundary conditions
            shift = domainPtr_->shift(i, j);

            if (domainPtr_->grid().dimension(i) > 1)  {
               bufferPtr_->clearSendBuffer();
               bufferPtr_->beginSendBlock(Buffer::GHOST);
            }

            sendArray_(i, j).clear();
            recvArray_(i, j).clear();

            // Loop over local atoms on this processor
            storagePtr_->begin(localIter);
            for ( ; !localIter.atEnd(); ++localIter) {

               // Decide whether to send as ghost.
               coord = localIter->position()[i];
               if (j == 0) {
                  assert(coord > bound);
                  choose = (coord < inner);
               } else {
                  assert(coord < bound);
                  choose = (coord > inner);
               }

               if (choose) {

                  sendArray_(i, j).append(*localIter);

                  if (domainPtr_->grid().dimension(i) > 1)  {

                     // Pack atom for sending 
                     bufferPtr_->packGhost(*localIter);

                  } else {  // if grid dimension == 1

                     // Make a ghost copy of the local atom on this processor
                     ptr = storagePtr_->newGhostPtr();
                     recvArray_(i, j).append(*ptr);
                     ptr->position() = localIter->position();
                     ptr->setTypeId(localIter->typeId());
                     ptr->setId(localIter->id());
                     if (shift) {
                        ptr->position()[i] += shift * lengths[i];
                     }
                     storagePtr_->addNewGhost();

                     #ifdef UTIL_DEBUG
                     // Validate shifted positions
                     if (j == 0) {
                        assert(ptr->position()[i] > domainPtr_->domainBound(i, 1));
                     } else {
                        assert(ptr->position()[i] < domainPtr_->domainBound(i, 0));
                     }
                     #endif

                  }

               } 

            }

            // Loop over ghosts on this processor, for resending.
            storagePtr_->begin(ghostIter);
            for ( ; !ghostIter.atEnd(); ++ghostIter) {

               // Decide whether to resend this ghost
               coord = ghostIter->position()[i];
               if (j == 0) {
                  choose = (coord > bound) && (coord < inner);
               } else {
                  choose = (coord < bound) && (coord > inner);
               }

               if (choose) {

                  sendArray_(i, j).append(*ghostIter);

                  if (domainPtr_->grid().dimension(i) > 1)  {
                     
                     // Pack ghost for resending
                     bufferPtr_->packGhost(*ghostIter);

                  } else {  // if grid dimension == 1

                     // Make another ghost copy on the same processor
                     ptr = storagePtr_->newGhostPtr();
                     recvArray_(i, j).append(*ptr);
                     ptr->position() = ghostIter->position();
                     ptr->setTypeId(ghostIter->typeId());
                     ptr->setId(ghostIter->id());
                     if (shift) {
                        ptr->position()[i] += shift * lengths[i];
                     }
                     storagePtr_->addNewGhost();

                     #ifdef UTIL_DEBUG
                     // Validate shifted position
                     if (j == 0) {
                        assert(ptr->position()[i] > domainPtr_->domainBound(i, 1));
                     } else {
                        assert(ptr->position()[i] < domainPtr_->domainBound(i, 0));
                     }
                     #endif

                  } 

               }

            }

            // Send and receive buffers
            if (domainPtr_->grid().dimension(i) > 1) {

               bufferPtr_->endSendBlock();

               source = domainPtr_->sourceRank(i, j);
               dest   = domainPtr_->destRank(i, j);
               bufferPtr_->sendRecv(domainPtr_->communicator(), source, dest);

               // Unpack ghosts and add to recvArray
               bufferPtr_->beginRecvBlock();
               while (bufferPtr_->recvSize() > 0) {

                  ptr = storagePtr_->newGhostPtr();
                  bufferPtr_->unpackGhost(*ptr);
                  if (shift) {
                     ptr->position()[i] += shift * lengths[i];
                  }
                  recvArray_(i, j).append(*ptr);
                  storagePtr_->addNewGhost();

                  #ifdef UTIL_DEBUG
                  // Validate ghost coordinates on receiving processor.
                  if (j == 0) {
                     assert(ptr->position()[i] > domainPtr_->domainBound(i, 1));
                  } else {
                     assert(ptr->position()[i] < domainPtr_->domainBound(i, 0));
                  }
                  #endif

               }

            }

         } // transmit direction j = 0, 1

      } // Cartesian index i = 0, ..., Dimension - 1

   }

   /*
   * Update ghost atom coordinates.
   *
   * Call on time steps for which no reneighboring is required. 
   */
   void Exchanger::updateGhosts()
   {
      Vector lengths = boundaryPtr_->lengths();
      Atom*  ptr;
      int    i, j, k, source, dest, size, shift;

      for (i = 0; i < Dimension; ++i) {
         for (j = 0; j < 2; ++j) {

            // Shift on receiving processor for periodic boundary conditions
            shift = domainPtr_->shift(i, j);

            if (domainPtr_->grid().dimension(i) > 1) {

               // Pack ghost positions for sending
               bufferPtr_->clearSendBuffer();
               bufferPtr_->beginSendBlock(Buffer::GHOST);
               size = sendArray_(i, j).size();
               for (k = 0; k < size; ++k) {
                  bufferPtr_->packGhost(sendArray_(i, j)[k]);
               }
               bufferPtr_->endSendBlock();
  
               // Send and receive buffers
               source = domainPtr_->sourceRank(i, j);
               dest   = domainPtr_->destRank(i, j);
               bufferPtr_->sendRecv(domainPtr_->communicator(), source, dest);
   
               // Unpack ghost positions
               bufferPtr_->beginRecvBlock();
               size = recvArray_(i, j).size();
               for (k = 0; k < size; ++k) {
                  ptr = &recvArray_(i, j)[k];
                  bufferPtr_->unpackGhost(*ptr);
                  if (shift) {
                     ptr->position()[i] += shift * lengths[i];
                  }
               }

            } else {

               // If grid().dimension(i) == 1, then copy positions of atoms
               // listed in sendArray to those listed in the recvArray.

               size = sendArray_(i, j).size();
               assert(size == recvArray_(i, j).size());
               for (k = 0; k < size; ++k) {
                  ptr = &recvArray_(i, j)[k];
                  ptr->position() = sendArray_(i, j)[k].position();
                  if (shift) {
                     ptr->position()[i] += shift * lengths[i];
                  }
               }

            }  

         } // transmit direction j = 0, 1

      } // Cartesian direction i = 0, ..., Dimension - 1

   }

   #endif

}
#endif