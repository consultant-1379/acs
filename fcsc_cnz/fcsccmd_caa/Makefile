# **********************************************************************
#
# Short description:
# Makefile template for XXX library
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
#                                                            
# **********************************************************************

#CXC_NR = CXC120333

CNZ_NAME ?= fcsc_cnz
CAA_NAME ?= fcsccmd_caa
CXC_NAME ?= fcsc_cxc

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(CURDIR)/../fcsccmd_caa/
OUTDIR   = $(CURDIR)/../fcsc_cxc/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES += -I$(INCDIR) -I$(LIB_CURL_SDK_INC) -I$(INCDIR) -I$(LIB_CURL_SDK_INC) -I$(COREMW_SDK_INC) -I$(AP_SDK_INC)/ -I$(LOTC_SDK_LIB)/  -I$(ACS_ROOT)/tra_cnz/traapi_caa/inc -I$(LOTC_SDK_CPP) -I$(ACE_SDK_PATH) -I$(LOTC_SDK_CPP)/x86_64-suse-linux -I$(CROSS_COMPILE)/

LDFLAGS      = -L.
LIBSDIR += -L$(LIB_CURL_SDK_LIB) -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/ -L$(COREMW_SDK_LIB)/ -L$(AP_SDK_LIB)/
## # here you can add own libs
LIBS = -lxml2 -lACE -lcurl -lidn -lACS_APGCC -llog4cplus #-lACS_TRA   

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
#

#SRCFILES = src/fcsc_Common.cpp src/fcsc_CompatibiltyChecker.cpp src/fcsc_DownloadMgr.cpp src/fcsc_RepositoryMgr.cpp src/fcsc_transfer.cpp src/show_off.cpp src/fcsc_prepare.cpp src/fcsc_Commands.cpp src/fcsc_Swprint.cpp 
#SRCFILES = $(wildcard $(SRCDIR)/*.cpp) 

SRCFILES = src/fcsc_Swprint.cpp
 
#OBJ = fcsc_Common.obj fcsc_CompatibiltyChecker.obj fcsc_DownloadMgr.obj fcsc_RepositoryMgr.obj fcscPrepare.obj fcsc_transfer.obj fcsc_Commands.obj fcsc_prepare.obj fcsc_Swprint.obj
OBJ = fcsc_Swprint.obj
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
#OBJ = \
#		$(SRCFILES:%.cpp=%.obj) \
#		$(SRCFILES:%.c=%.obj)
 
%.obj:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Compiling file: $*.cpp'
	$(CC) $(CFLAGS) -c -g -O0 $(APGINCLUDE) $(CINCLUDES) src/$*.cpp -o $(OBJDIR)/$@
	$(SEPARATOR_STR)

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME1 = fcsctransfer

APEXE1 = $(OUTDIR)/$(APNAME1)

APNAME2 = fcscprepare

APEXE2 = $(OUTDIR)/$(APNAME2)

APNAME3 = swprint

APEXE3 = $(OUTDIR)/$(APNAME3)

APNAME4 = fcscUpdSdpList

APEXE4 = $(OUTDIR)/$(APNAME4)

TEST = $(OUTDIR)/test

#.PHONY: depend clean
.PHONY: clean
#Build variables
FCSCPREP = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_CompatibiltyChecker.obj $(OBJDIR)/fcsc_DownloadMgr.obj $(OBJDIR)/fcsc_RepositoryMgr.obj $(OBJDIR)/fcscPrepare.obj
FCSCXFER = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_CompatibiltyChecker.obj $(OBJDIR)/fcsc_DownloadMgr.obj $(OBJDIR)/fcsc_RepositoryMgr.obj $(OBJDIR)/fcsc_transfer.obj $(OBJDIR)/fcsc_prepare.obj $(OBJDIR)/fcsc_Commands.obj
DWNDMGR = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_DownloadMgr.obj
#REPOMGR = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_RepositoryMgr.obj 
UPDSDPLST = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_UpdSdpList.obj
SWPRINT = $(OBJDIR)/fcsc_Swprint.obj
TST = $(OBJDIR)/test.obj
 
all:  
#all: $(APEXE2) $(APEXE1) $(APEXE3)
#all: $(TEST)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

$(TEST): $(OBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    #$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(CC) -o $(TEST) $(TST) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(APEXE1): $(OBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    #$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(CC) -o $(APEXE1) $(FCSCXFER) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(APEXE2): $(OBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    #$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(CC) -o $(APEXE2) $(FCSCPREP) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(APEXE3): $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	#$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	$(CC) -o $(APEXE3) $(SWPRINT) $(LIBSDIR) $(LIBS)
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

metrics : cccc lint

.PHONY: cccc
cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

