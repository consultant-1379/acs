# **********************************************************************
#
# Short description:
# Makefile template for DSD Private Implementation Library
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2010-02-19   teiggam   Created
#            2010-07-30   xnicmut   Adjusted
# **********************************************************************

# Here you can specify what kind od product will be compiled. Use
#   so for generate a shared object library
#   app for generate an executable application
# By default, this make will generate a shared object library.
# You can also specify these variable as arguments of make command.
# For example: make PROD_TYPE=app
# Here you can specify what kind od product will be compiled. Use
#   so for generate a shared object library
#   app for generate an executable application
# By default, this make will generate a shared object library.
# You can also specify these variable as arguments of make command.
# For example: make PROD_TYPE=app
PROD_TYPE ?= app
      
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 

# Clearcase name organizations
CNZ_NAME ?= apgcc_cnz
CAA_NAME ?= apgccapi_caa
CXC_NAME ?= apgcc_cxc

# Basic paths
#VOB_PATH = /vobs/cm4ap/$(VOB_NAME)
#CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
#CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

WORK_PATH = $(PWD)
#CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
#CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

# Coverage compiler flag: add this flag to make command to produce outputs with coverage.
# By default coverage is disabled.
# Example to activate compilation with coverage: make COVERAGE=1
COVERAGE ?= no

include $(COMMON_ROOT)/common.mk 

OUTDIR = $(WORK_PATH)/bin
OUTOBJ = $(WORK_PATH)/obj
OBJDIR = obj
SRCDIR = .
INCDIR = $(WORK_PATH)/inc
TESTDIR =
DEMODIR =

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make SO_NAME=ACS_DSD2 SO_RELEASE_BUILD=1
SO_NAME ?=
SO_RELEASE_MAJOR ?=
SO_RELEASE_MINOR ?=
SO_RELEASE_BUILD ?=

# If the product is an application, here you can specify its name
# You can also specify these variable as arguments of make command.
# For example: make APP_NAME=dsdd
APP_NAME ?= OmHandler_Demo_new

COPY_TO_LOTC ?= no_copy

EXTRA_DEFINES ?=

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Source files
SRCFILES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCFILES += $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

## Object files
OBJFILES =	$(wildcard $(OBJDIR)/*.obj)

## Setting the so library compiler information
SO_VERSION ?= $(SO_RELEASE_MAJOR).$(SO_RELEASE_MINOR).$(SO_RELEASE_BUILD)
SO_FILENAME = lib$(SO_NAME).so.$(SO_VERSION)
SO_SONAME = lib$(SO_NAME).so.$(SO_RELEASE_MAJOR)
SO_LINKER_NAME = lib$(SO_NAME).so

## Setting the application compiler information
APP_FILENAME = $(APP_NAME)

# here you can add own lib paths the compiler visit before that of the common makefile
#LIBSDIR_BEFORE ?= -L"$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/OMHandling/obj"
LIBSDIR_BEFORE ?= -L"$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/obj"

# here you can add own lib paths the compiler visit before that of the common makefile
LIBSDIR_AFTER ?=

## here you can add own lib paths
#LIBSDIR +=

## here you can add own compiler flags
#CFLAGS += -O3 -pedantic -fPIC -fmessage-length=0 -std=gnu++0x
CFLAGS += -fPIC -fmessage-length=0

ifeq ($(COVERAGE),yes)
CCOVFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CCOVFLAGS = -O3
endif

# here you can add own linker flags
LDFLAGS += -s

ifeq ($(PROD_TYPE),so)
LDFLAGS += -shared -Wl,-soname=$(SO_SONAME)
PROD_FILENAME = $(SO_FILENAME)
else
PROD_FILENAME = $(APP_FILENAME)
endif

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDE_BEFORE ?= -I"$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/OMHandling/inc"

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"
CINCLUDES += -I"$(WORK_PATH)/cute"

# here you can add more defines
#DEFINES += $(EXTRA_DEFINES)
EXTRA_CFLAGS += $(EXTRA_DEFINES) $(CINCLUDE_BEFORE)

# here you can add own libs
#LIBS += -lrt -lpthread -lACS_APGCC -lACS_TRA -lACS_APGCC_RuntimeTest "$(ACS_ROOT)/tra_cnz/tra_cxc/bin/liblog4cplus.so" 
LIBS += -lrt -lpthread -lACS_APGCC -lACS_TRA "$(ACS_ROOT)/tra_cnz/tra_cxc/bin/liblog4cplus.so"

#LIBS += -lrt -lpthread -lACS_APGCC

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
			$(SRCFILES:%.c=%.obj)

#.PHONY: depend clean
.PHONY: clean

all: CFLAGS += $(CCOVFLAGS)
all: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
ifeq ($(PROD_TYPE),so)
		$(SILENT)$(ECHO) 'Creating library: $(PROD_FILENAME)'
else
		$(SILENT)$(ECHO) 'Creating Application: $(PROD_FILENAME)'
endif
	$(SILENT)$(RM) $(OBJDIR)/$(PROD_FILENAME)
	$(CC) $(CFLAGS) -o $(OBJDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR_BEFORE) $(LIBSDIR) $(LIBSDIR_AFTER) $(LIBS)
	$(SEPARATOR_STR)
ifneq ($(COPY_TO_LOTC),no_copy)
	$(SILENT) sftp -b copy-to-simlotc.batch $(COPY_TO_LOTC) >&/dev/null
endif
	$(NEW_LINE)

release: CFLAGS += -O3
release: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
ifeq ($(PROD_TYPE),so)
		$(SILENT)$(ECHO) 'Creating library: $(PROD_FILENAME)'
else
		$(SILENT)$(ECHO) 'Creating Application: $(PROD_FILENAME)'
endif
	$(SILENT)$(RM) $(OUTDIR)/$(PROD_FILENAME)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR_BEFORE) $(LIBSDIR) $(LIBSDIR_AFTER) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

clean:
	$(SILENT)$(ECHO) 'Cleaning file: $@'
	$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

#metrics : cccc lint
#
#cccc:
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
##		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
#		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
#	    $(SEPARATOR_STR)
#
#lint:
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'LINT file: $@'
##		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
#		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
#	    $(SEPARATOR_STR)

depend:	$(SRCFILES)
	makedepend $(CINCLUDES) $^

## DO NOT DELETE THIS LINE -- make depend needs it

