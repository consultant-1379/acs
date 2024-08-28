# **********************************************************************
#
# Short description:
# Makefile template for TRAPDS service binary
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
#            2010-08-27   xgalrob   Created
#           
# **********************************************************************

CNZ_NAME ?= trapds_cnz
CAA_NAME ?= trapdscmd_caa
CXC_NAME ?= trapds_cxc

include $(COMMON_ROOT)/common.mk 

CAA_NAME = trapdscmd_caa
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

# here you can add own lib paths
LIBSDIR  +=

CFLAGS += -fPIC -fmessage-length=0 -Wstrict-overflow=0
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
#CINCLUDES += -Icsprot/inc
GCOV_FLAGS =
LIBS += -lgcov
endif

LDFLAGS += 
TRAPDSSRV_APNAME = showpkg
TRAPDSSRV_APBIN = $(OUTDIR)/$(TRAPDSSRV_APNAME)

# here you can add own Include paths and/or other includes
#CINCLUDES += -I inc -I trapdsprot/inc -I ../trapdsapi_caa/inc_ext -I ../trapdsapi_caa/inc_int -I../trapdscmd_caa/inc \
#-I common/inc -I ../trapdsadm_caa/inc

CINCLUDES += -I inc -I ../trapdscmd_caa/inc -I ../trapdsapi_caa/inc_ext -I ../trapdsapi_caa/inc_int \
-I ../trapdsadm_caa/inc

CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/
CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ace
CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ASNMP
CINCLUDES += -I$(IODEV_VOB)/AP_SDK/Include

#CINCLUDES += -I/Home/eanform/Desktop/xerces-c-3.1.1-x86-linux-gcc-3.4/xerces-c-3.1.1-x86-linux-gcc-3.4/include

LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR) -L$(CXC_PATH)/ext_lib -L$(IODEV_VOB)/AP_SDK/Lib 
LIBS += -lrt -lpthread -ldhcpctl -lomapi -ldst -lacs_tra -lacs_apgcc -lacs_dsd

SOURCES += 
	
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)

	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR) src ../trapdsapi_caa/src ../trapdscmd_caa/src ../trapdsadm_caa/src
vpath %.h $(INCDIR) inc trapdsprot/inc ../trapdsapi_caa/inc_int ../trapdsapi_caa/inc_ext ../trapdsadm_caa/inc
vpath %.obj $(OBJDIR)
#temporary added below
#vpath %.cpp trapdsprot/src

#Build
.PHONY: all
all: $(TRAPDSSRV_APNAME)

#Build + check-in check-out
.PHONY: buildall
buildall: checkout
buildall: $(TRAPDSSRV_APNAME)
buildall: checkin

#Create binary
.PHONY: $(TRAPDSSRV_APNAME)
$(TRAPDSSRV_APNAME): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating executable : $(TRAPDSSRV_APNAME)'
#   $(SILENT)$(RM) -f $(TRAPDSSRV_APBIN)
#	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(TRAPDSSRV_APBIN) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(CC) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $(TRAPDSSRV_APBIN)
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
# Perform prebuild actions like ClearCase checkouts
# Perform postbuild actions like ClearCase checkins
# Undo prebuild actions like ClearCase checkouts
.PHONY: undoprebuild
undoprebuild: uncheckout	

# ClearCase checkouts
.PHONY: checkout
checkout:	
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking out TRAPDS srv binary $(TRAPDSSRV_APBIN)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(TRAPDSSRV_APBIN))))' != 'CHECKEDOUT' ]; then cleartool co -nc $(TRAPDSSRV_APBIN); else echo 'Binary is already checked out!'; fi
	$(NEW_LINE)

# ClearCase checkins
.PHONY: checkin
checkin:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking in TRAPDS srv binary $(TRAPDSSRV_APBIN)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(TRAPDSSRV_APBIN))))' = 'CHECKEDOUT' ]; then cleartool ci -nc $(TRAPDSSRV_APBIN); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)

# Undo checkouts
.PHONY: uncheckout
uncheckout:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Undoing checkout of TRAPDS srv binary $(TRAPDSSRV_APBIN)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(TRAPDSSRV_APBIN))))' = 'CHECKEDOUT' ]; then cleartool uncheckout -rm $(TRAPDSSRV_APBIN); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)

#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning SRV build files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning SRV binary files: $(TRAPDSSRV_APNAME)'
	$(SILENT)$(RM) -f $(OUTDIR)/$(TRAPDSSRV_APNAME)

# Perform chain of code analysis tools - add more?
.PHONY: metrics
metrics : cccc lint

# CCCC
# C and C++ Code Counter
.PHONY: cccc
cccc:
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(INCDIR)/*) 
	$(CCCC) $(wildcard $(SRCDIR)/*) 

# LINT
# Splint is a tool for static code analysis
.PHONY: lint
lint:
	$(SILENT)$(ECHO) 'LINT file: $@'
	$(LINT) $(wildcard $(INCDIR)/*) 
	$(LINT) $(wildcard $(SRCDIR)/*)     

# Extract debug symbols
.PHONY: symbols
symbols:
	$(SILENT)$(ECHO) 'Extracting symbols from $(TRAPDSSRV_APNAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --only-keep-debug $(OUTDIR)/$(TRAPDSSRV_APNAME) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --strip-debug $(OUTDIR)/$(TRAPDSSRV_APNAME)
	$(SILENT)$(OBJCOPY) --add-gnu-debuglink=$(OUTDIR)/$(TRAPDSSRV_APNAME).dbg $(OUTDIR)/$(TRAPDSSRV_APNAME)
	readelf -n $(OUTDIR)/$(TRAPDSSRV_APNAME)
#TODO: RENAME AND MOVE ACCORDING TO DR SS5
	$(SILENT)$(MV) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg $(CXCDIR)/debug/SymbolFiles/$(TRAPDSSRV_APNAME).debug
	
		
#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it

