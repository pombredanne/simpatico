SRC_DIR_REL =../../../src
include $(SRC_DIR_REL)/compiler.mk

#-----------------------------------------------------------------------
EXES=example1 example2

ifdef UTIL_MPI
EXES+=example3
endif

#-----------------------------------------------------------------------
all: $(EXES)

clean:
	rm -f $(EXES)
#-----------------------------------------------------------------------
# Path(s) to search for header files. 
INCLUDES= -I$(TESTS_DIR)/test

# Extra dependencies for all source files
ALLDEPS= $(SRC_DIR)/compiler.mk ../UnitTest.h ../TestRunner.h \
         ../UnitTestRunner.h ../CompositeTestRunner.h
#-----------------------------------------------------------------------

example1: example1.cpp $(ALLDEPS)
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) $(INCLUDES) -o $@ $<
	
example2: example2.cpp $(ALLDEPS)
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) $(INCLUDES) -o $@ $<

example3: example3.cpp $(ALLDEPS)
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) $(INCLUDES) -o $@ $<
	
