#ifndef UTIL_EXCEPTION_CPP
#define UTIL_EXCEPTION_CPP

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010 - 2012, David Morse (morse012@umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "Exception.h"
#include "Log.h"

#include <sstream>

namespace Util
{

   /*
   * Constructor, with no function name.
   */
   Exception::Exception(const char *function, const char *message, 
                        const char *file, int line, int echo) 
    : message_()
   {
      message_ = "   Function: ";
      message_.append(function);
      message_.append("\n");
      message_.append("   Message : ");
      message_.append(message);
      message_.append("\n");
      message_.append("   File    : ");
      message_.append(file);
      message_.append("\n");
      message_.append("   Line    : ");
      std::ostringstream s;
      s << line;
      message_.append(s.str());
      message_.append("\n");
      if (echo) {
        write(Log::file());
      }
   }

   /*
   * Constructor, with no function name.
   */
   Exception::Exception(const char *message, 
                        const char *file, int line, int echo) 
    : message_()
   {
      message_ = "   Message : ";
      message_.append(message);
      message_.append("\n");
      message_.append("   File    : ");
      message_.append(file);
      message_.append("\n");
      message_.append("   Line    : ");
      std::ostringstream s;
      s << line;
      message_.append(s.str());
      message_.append("\n");
      if (echo) {
         write(Log::file());
      }
   }

   /*
   * Destructor.
   */
   Exception::~Exception()
   {}

   /*
   * Write message to stream.
   */
   void Exception::write(std::ostream &out)
   {  out << message_; }

   /*
   * Get message by reference
   */
   std::string& Exception::message()
   {  return message_; }


} 
#endif
