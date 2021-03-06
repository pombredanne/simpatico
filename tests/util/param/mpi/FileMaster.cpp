#ifndef FILE_MASTER_CPP
#define FILE_MASTER_CPP

/*
* Simpatico - Simulation Package for Polymeric and Molecular Liquids
*
* Copyright 2010, David Morse (morse@cems.umn.edu)
* Distributed under the terms of the GNU General Public License.
*/

#include "FileMaster.h"
#include <util/global.h>

#include <sstream>

namespace McMd
{

   /* 
   * Default constructor.   
   */
   FileMaster::FileMaster() 
   : commandFileName_(),
     inputPrefix_(),
     outputPrefix_(),
     directoryIdPrefix_(),
     paramFilePtr_(0),
     commandFilePtr_(0),
     hasDirectoryId_(false),
     isSetParamFileStdIn_(false)
   {}

   /* 
   * Copy constructor.   
   */
   FileMaster::FileMaster(const FileMaster& other) 
   : inputPrefix_(other.inputPrefix_),
     outputPrefix_(other.outputPrefix_),
     directoryIdPrefix_(other.directoryIdPrefix_),
     paramFilePtr_(0),
     hasDirectoryId_(other.hasDirectoryId_),
     isSetParamFileStdIn_(other.isSetParamFileStdIn_)
   {}

   /* 
   * Destructor.   
   */
   FileMaster::~FileMaster() 
   {
      if (paramFilePtr_) {
         paramFilePtr_->close();
         delete paramFilePtr_;
      }
      if (commandFilePtr_) {
         commandFilePtr_->close();
         delete commandFilePtr_;
      }
   }

   /*
   * Set a directory Id prefix.
   */
   void FileMaster::setDirectoryId(int rank)
   {
      std::stringstream sMyId;
      sMyId << rank;
      directoryIdPrefix_ = sMyId.str();
      directoryIdPrefix_ += "/";
      hasDirectoryId_ = true;
   }

   /*
   * Set paramFile() to return std::cin even if a directory id has been set.
   */
   void FileMaster::setParamFileStdIn()
   {  isSetParamFileStdIn_ = true; }

   /* 
   * Set the input prefix.
   */
   void FileMaster::setInputPrefix(const std::string& inputPrefix) 
   {  inputPrefix_ = inputPrefix; }

   /* 
   * Set the output prefix.
   */
   void FileMaster::setOutputPrefix(const std::string& outputPrefix) 
   {  outputPrefix_ = outputPrefix; }

   /* 
   * Read parameters from file.
   */
   void FileMaster::readParam(std::istream &in) 
   {
      readBegin(in, "FileMaster");
      read<std::string>(in, "commandFileName",  commandFileName_);
      read<std::string>(in, "inputPrefix",  inputPrefix_);
      read<std::string>(in, "outputPrefix", outputPrefix_);
      readEnd(in);
   }

   /*
   * Get the default parameter stream.
   */
   std::istream& FileMaster::paramFile()
   {

      if ((!hasDirectoryId_) || isSetParamFileStdIn_) { 

         return std::cin;

      } else { 

         if (!paramFilePtr_) { 

            // Construct filename "n/param" for processor n
            std::string filename(directoryIdPrefix_);
            filename += "param";
      
            // Open parameter input file
            std::ifstream* filePtr = new std::ifstream();
            filePtr->open(filename.c_str());
            if (filePtr->fail()) {
               std::string message = "Error opening parameter file. Filename: ";
               message += filename;
               UTIL_THROW(message.c_str());
            }
            paramFilePtr_ = filePtr;

         }

         return *paramFilePtr_;
      }
   }

   /*
   * Get the command input stream by reference.
   */
   std::istream& FileMaster::commandFile()
   {

      if (commandFileName_ == "paramfile") {

         return paramFile();

      } else { 

         if (!commandFilePtr_) { 

            // Construct filename "n/param" for processor n
            std::string filename(directoryIdPrefix_);
            filename += commandFileName_;
      
            // Open parameter input file
            std::ifstream* filePtr = new std::ifstream();
            filePtr->open(filename.c_str());
            if (filePtr->fail()) {
               std::string message = "Error opening command file. Filename: ";
               message += filename;
               UTIL_THROW(message.c_str());
            }
            commandFilePtr_ = filePtr;

         }

         return *commandFilePtr_;
      }
   }
   /*
   * Open and return an input file named inputPrefix + name
   */
   void
   FileMaster::openInputFile(const std::string& name, std::ifstream& in) const
   {
      // Construct filename = inputPrefix_ + name
      std::string filename;
      if (hasDirectoryId_) {
         filename  = directoryIdPrefix_;
      }
      filename += inputPrefix_;
      filename += name;

      in.open(filename.c_str());

      // Check for error opening file
      if (in.fail()) {
         std::string message = "Error opening input file. Filename: ";
         message += filename;
         UTIL_THROW(message.c_str());
      }

   }

   /*
   * Open and return an output file named outputPrefix + name
   */
   void
   FileMaster::openOutputFile(const std::string& name, std::ofstream& out) 
   const
   {
      // Construct filename = outputPrefix_ + name
      std::string filename;
      if (hasDirectoryId_) {
         filename  = directoryIdPrefix_;
      }
      filename += outputPrefix_;
      filename += name;

      out.open(filename.c_str());

      // Check for error opening file
      if (out.fail()) {
         std::string message = "Error opening output file. Filename: ";
         message += filename;
         UTIL_THROW(message.c_str());
      }

   }

   /*
   * Will paramFile() return std::cin ?
   */ 
   bool FileMaster::isParamFileStdIn() const
   {  return ((!hasDirectoryId_) || isSetParamFileStdIn_); }

} 
#endif
