# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

# Clearcase name organizations
CNZ_NAME ?= apbm_cnz
CAA_NAME ?= apbmapi_caa
CXC_NAME ?= apbm_cxc

# Basic paths
VOB_PATH ?= $(ACS_ROOT)
CNZ_PATH ?= $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH ?= $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH ?= $(CNZ_PATH)/$(CXC_NAME)
CXCDIR ?= $(CXC_PATH)

include $(COMMON_ROOT)/common.mk

# Coverage compiler flag: add this flag to make command to produce outputs with coverage.
# By default coverage is disabled.
# Example to activate compilation with coverage: make COVERAGE=1
COVERAGE ?= no

OUTDIR ?= $(CXC_PATH)/bin
OUTOBJ ?= $(CAA_PATH)/obj
OBJDIR ?= obj
SRCDIR ?= src
INCDIR ?=
TESTDIR ?=
DEMODIR ?=

#### If the product is a shared object library, here you can specify the name and the version numbers of the library
#### You can also specify these variable as arguments of make command.
#### For example: make SO_NAME=ACS_DSD2 SO_RELEASE_BUILD=1
###SO_NAME ?=
###SO_RELEASE_MAJOR ?=
###SO_RELEASE_MINOR ?=
###SO_RELEASE_BUILD ?=

# If the product is an application, here you can specify its name
# You can also specify these variable as arguments of make command.
# For example: make APP_NAME=dsdd
APP_NAME ?= generic-test

###COPY_TO_LOTC ?= no_copy

EXTRA_DEFINES ?=

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= $(SRCDIR)

## Source files
SRCFILES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCFILES += $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

## Object files
OBJFILES =	$(wildcard $(OBJDIR)/*.obj)

##### Setting the so library compiler information
###SO_VERSION ?= $(SO_RELEASE_MAJOR).$(SO_RELEASE_MINOR).$(SO_RELEASE_BUILD)
###SO_FILENAME = lib$(SO_NAME).so.$(SO_VERSION)
###SO_SONAME = lib$(SO_NAME).so.$(SO_RELEASE_MAJOR)
###SO_LINKER_NAME = lib$(SO_NAME).so

## Setting the application compiler information
APP_FILENAME = $(APP_NAME)

# here you can add own lib paths
LIBSDIR += -L"$(LIB_EXT)"

## here you can add own compiler flags
#CFLAGS += -O3 -pedantic -fPIC -fmessage-length=0 -std=gnu++0x
#CFLAGS += -fPIC -fmessage-length=0

ifeq ($(COVERAGE),yes)
CCOVFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CCOVFLAGS = -O3
endif

# here you can add own linker flags
#LDFLAGS += -s

PROD_FILENAME = $(APP_FILENAME)

# here you can add own Include paths and/or other includes
#CINCLUDES += -I"../inc"  -I"$(CAA_PATH)/inc" -I"$(CNZ_PATH)/cute" -I"/usr/include64/boost_1_44_0"
CINCLUDES += -I"src" -I"$(CAA_PATH)/$(INCEXTDIR)" -I"$(CAA_PATH)/$(INCINTDIR)"

# here you can add more defines
DEFINES += $(EXTRA_DEFINES)

# here you can add own libs
LIBS += -lrt -lpthread -lacs_apbm

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
			$(SRCFILES:%.c=%.obj)

#.PHONY: depend clean
.PHONY: clean

all: CFLAGS += $(CCOVFLAGS)
all: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PROD_FILENAME)'
	$(SILENT)$(RM) $(OBJDIR)/$(PROD_FILENAME)
	$(CC) $(CFLAGS) -o $(OBJDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

release: CFLAGS += -O3
release: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PROD_FILENAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(PROD_FILENAME)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(PROD_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
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
