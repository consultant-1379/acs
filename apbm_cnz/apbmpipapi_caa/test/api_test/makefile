#06/set/11
#3:29:25 p.: Giovanni Papale: # **********************************************************************
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
PROD_TYPE ?= app

# Clearcase name organizations
CNZ_NAME ?= apbm_cnz
CAA_NAME ?= apbmadm_caa
CXC_NAME ?= apbm_cxc

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

# Coverage compiler flag: add this flag to make command to produce outputs with coverage.
# By default coverage is disabled.
# Example to activate compilation with coverage: make COVERAGE=1
COVERAGE ?= 0

include $(COMMON_ROOT)/common.mk

OUTDIR = $(CXC_PATH)/bin
OBJDIR = obj
SRCDIR = src
INCDIR = inc
TESTDIR = test
DEMODIR = demo

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

APP_NAME ?= apbm_pipapi_test

COPY_TO_LOTC ?= 0

EXTRA_DEFINES ?=

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Source files
#SRCFILES =	$(wildcard $(SRCDIR)/*.cpp)
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

# here you can add own lib paths
#LIBSDIR += -L"$(CNZ_PATH)/traapi_caa/obj"
#LIBSDIR += -L"$(CXC_PATH)/bin"
LIBSDIR += -L$(LIB_SSH2_SDK_LIB)
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"
LIBSDIR += -L"$(CXC_PATH)/bin/lib_ext"


## here you can add own compiler flags
#CFLAGS += -O3 -pedantic -fPIC -fmessage-length=0 -std=gnu++0x
CFLAGS += -O3 -fPIC -fmessage-length=0 

ifeq ($(COVERAGE),1)
CCOVFLAGS += -fprofile-arcs -ftest-coverage
else
CCOVFLAGS =
endif

# here you can add own linker flags
LDFLAGS += -s

ifeq ($(PROD_TYPE),so)
LDFLAGS += -shared -Wl,-soname=$(SO_SONAME)
PROD_FILENAME = $(SO_FILENAME)
else
PROD_FILENAME = $(APP_FILENAME)
endif

# here you can add own Include paths and/or other includes
CINCLUDES += -I$(CAA_API_DIR)
CINCLUDES += $(ACS_ROOT)/apbm_cnz/apbmpipapi_caa/inc_ext


# libssh2 include files
CINCLUDES += -I$(LIB_SSH2_SDK_INC)
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"


# here you can add more defines
#DEFINES += $(EXTRA_DEFINES)
EXTRA_CFLAGS += $(EXTRA_DEFINES) $(CINCLUDES_BEFORE)

# here you can add own libs
#LIBS += -lrt -lpthread /vobs/IO_Developments/BOOST_SDK/boost_1_44_0/lib/libboost_serialization.so.1.44.0  /vobs/IO_Developments/AP_SDK/Lib/libacs_csapi.so.3.7.0 /vobs/cm4ap/ntacs/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbmpip.so.1.0.0 -lacs_dsd -lacs_prc -lacs_apgcc -lacs_tra -llog4cplus -lacs_aeh 
#LIBS += -lrt -lpthread /vobs/cm4ap/ntacs/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbmpip.so.1.0.0  -lacs_tra -llog4cplus
LIBS += -lrt -lpthread -lacs_apbmpip -lacs_tra -llog4cplus
#/vobs/cm4ap/ntacs/tra_cnz/tra_cxc/bin/libacs_tra.so
#LIBS +=  /vobs/IO_Developments/AP_SDK/Lib/libacs_csapi.so.3.7.0

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
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(OBJDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR_BEFORE) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
ifeq ($(COPY_TO_LOTC),1)
	$(SILENT) sftp -b copy-to-simlotc.batch root@192.168.254.101 >&/dev/null
endif
	$(NEW_LINE)

release: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
ifeq ($(PROD_TYPE),so)
		$(SILENT)$(ECHO) 'Creating library: $(PROD_FILENAME)'
else
		$(SILENT)$(ECHO) 'Creating Application: $(PROD_FILENAME)'
endif
	$(SILENT)$(RM) $(PROD_FILENAME)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(OUTDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR_BEFORE) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

clean:
	$(SILENT)$(ECHO) 'Cleaning file: $@'
	$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

depend:	$(SRCFILES)
	makedepend $(CINCLUDES) $^

## DO NOT DELETE THIS LINE -- make depend needs it
