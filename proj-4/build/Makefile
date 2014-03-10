# Makefile for CMSC 23700 Project 4
# Winter 2014
#

TARGET =	proj-4

SHELL =		/bin/sh
OS =		$(shell uname -s)

# the location of doxygen depends on the system that you are running,
# but you may have to edit this path.
#
ifeq ($(OS),Darwin)
  # standard location for MacLab machines
  DOXYGEN =	/Applications/Doxygen.app/Contents/Resources/doxygen
else
  DOXYGEN =	/usr/bin/doxygen
endif

BUILD_DIR =	$(shell pwd)
COMMON_DIR =	../../common
JSON_DIR =	$(COMMON_DIR)/json
TQT_DIR =	$(COMMON_DIR)/tqt
DATA_DIR =	$(BUILD_DIR)/../data

LIB_INCLUDE_DIRS =	-I$(COMMON_DIR)/include -I$(TQT_DIR) -I$(JSON_DIR)

ifeq ($(OS),Darwin)
  CPPFLAGS = $(LIB_INCLUDE_DIRS) -I/opt/local/include -I/usr/local/include
  LDFLAGS = -L/opt/local/lib -L/usr/local/lib -lpng -lglfw -framework OpenGL -framework Foundation
  CXX = clang++ -std=c++11
else
  CPPFLAGS = $(LIB_INCLUDE_DIRS) -I/usr/local/include
  LDFLAGS = —L/usr/local/lib -lpng -lglfw -lGL -lGLU -lm
  CXX = g++
endif
CPPFLAGS +=	-DDATA_DIR=\"$(DATA_DIR)\"
#CXXFLAGS =	-O2 -Wall -pedantic
CXXFLAGS =	-g -Wall -pedantic

# where to find the source code
#
VPATH =		../src $(COMMON_DIR)/cs237-lib $(TQT_DIR) $(JSON_DIR)

SRCS =		$(wildcard ../src/*.cxx) \
		$(wildcard $(COMMON_DIR)/cs237-lib/*.cxx) \
		$(wildcard $(TQT_DIR)/*.cxx) \
		$(wildcard $(JSON_DIR)/*.cxx)
INCLUDES =	$(wildcard ../src/*.hxx)
ALL_INCLUDES =	$(INCLUDES) $(wildcard $(COMMON_DIR)/include/*.hxx) $(wildcard $(TQT_DIR)/*.hxx) $(wildcard $(JSON_DIR)/*.hxx)
DOC_SRCS =	$(SRCS) $(INCLUDES) ../main-page

OBJS =		$(notdir $(SRCS:.cxx=.o))

$(TARGET):	$(OBJS) .depend
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# include-file dependency information
#
.depend:	$(SRCS) $(ALL_INCLUDES)
	- $(CXX) $(CPPFLAGS) -MM $(SRCS) > .depend

ifneq ($(MAKECMDGOALS),clean)
sinclude .depend
endif

# generate documentation from sources using doxygen
#
.PHONY:		doc
doc:		../doc

../doc:		$(DOC_SRCS)
	(cd ..; $(DOXYGEN) Doxyfile)
	- touch ../doc

# cleanup by removing generated files
#
.PHONY:		clean
clean:
		rm -rf *.o $(TARGET) ../doc .depend

