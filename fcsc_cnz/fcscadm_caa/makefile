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
CAA_NAME ?= fcscadm_caa
CXC_NAME ?= fcsc_cxc

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(CURDIR)/../fcscadm_caa/
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

CINCLUDES += -I$(INCDIR) -I$(LIB_CURL_SDK_INC) -I$(COREMW_SDK_INC) -I$(AP_SDK_INC)/ -I$(LOTC_SDK_LIB)/  -I$(ACS_ROOT)/tra_cnz/traapi_caa/inc -I$(LOTC_SDK_CPP)                  -I$(ACE_SDK_PATH) -I$(LOTC_SDK_CPP)/x86_64-suse-linux  
CINCLUDES += -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/ha/app_mngr/inc
CINCLUDES += -I/vobs/IO_Developments//ACE_SDK/ACE_wrappers

CINCLUDES += -I$(ACE_SDK_PATH)/ace/os_include/netinet/

LDFLAGS      = -L. #-L/home/tcsvijk/

LIBSDIR += -L/. -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/ -L$(ACS_ROOT)/apgcc_cnz/apgccapi_cxc/bin/ -L$(COREMW_SDK_LIB)/
#LIBSDIR += -L$(LIB_CURL_SDK_LIB) -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/

## # here you can add own libs 
LIBS = -lxml2 -lACE   -llog4cplus  "/home/tcsdech/libACS_APGCC.so.2.0.0" "$(ACS_ROOT)/tra_cnz/tra_cxc/bin/libACS_TRA.so.1.1.1" 
#LIBS = -lxml2 -lACE   -llog4cplus  "$(ACS_ROOT)/apgcc_cnz/apgccapi_cxc/bin/libACS_APGCC.so.3.0.0" "$(ACS_ROOT)/tra_cnz/tra_cxc/bin/libACS_TRA.so.1.1.1" 

#LIBS = -lxml2 -lACE  -lACS_APGCC -lACS_TRA -llog4cplus  "$(ACS_ROOT)/apgcc_cnz/apgccapi_cxc/bin/libACS_APGCC.so.3.0.0"
#LIBS = -lxml2 -lACE -lcurl "$(ACS_ROOT)/fcsc_cnz/fcsccmd_caa/libACS_APGCC.so.3.0.0" -lACS_TRA 

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
#

SRCFILES =  src/fcsc_application.cpp  src/fcsc_OI_Handler.cpp  src/acsfcscd.cpp src/fcscadm_Common.cpp 

OBJ = fcsc_application.obj  fcsc_OI_Handler.obj  acsfcscd.obj fcscadm_Common.obj 

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
APNAME1 = fcscHaService

APEXE1 = $(OUTDIR)/$(APNAME1)

APNAME2 = fcscservice

APEXE2 = $(OUTDIR)/$(APNAME2)

APNAME3 = fcscRepoMgr

APEXE3 = $(OUTDIR)/$(APNAME3)

APNAME4 = fcscUpdSdpList

APEXE4 = $(OUTDIR)/$(APNAME4)

TEST = $(OUTDIR)/test

SWPRINT =$(OUTDIR)/swprint

#.PHONY: depend clean
.PHONY: clean
#Build variables

FCSCSERVICE =  $(OBJDIR)/fcsc_debug_svc_loader.obj $(OBJDIR)/fcsc_coremw_svc_loader.obj $(OBJDIR)/fcsc_svc_loader.obj $(OBJDIR)/fcsc_service.obj $(OBJDIR)/ACS_SW_OIHandler.obj  $(OBJDIR)/fcsc_OI_Operation.obj

FCSCOMCLIENT =   $(OBJDIR)/fcsc_application.obj  $(OBJDIR)/fcsc_OI_Handler.obj  $(OBJDIR)/acsfcscd.obj $(OBJDIR)/fcscadm_Common.obj

FCSCPREP = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_CompatibiltyChecker.obj $(OBJDIR)/fcsc_DownloadMgr.obj $(OBJDIR)/fcsc_RepositoryMgr.obj $(OBJDIR)/fcscPrepare.obj $(OBJDIR)/fcscprepare.obj
FCSCXFER = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_CompatibiltyChecker.obj $(OBJDIR)/fcsc_DownloadMgr.obj $(OBJDIR)/fcsc_RepositoryMgr.obj $(OBJDIR)/fcscTransfer.obj
DWNDMGR = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_DownloadMgr.obj
REPOMGR = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_RepositoryMgr.obj 
UPDSDPLST = $(OBJDIR)/fcsc_Common.obj $(OBJDIR)/fcsc_UpdSdpList.obj
FCSCSWPRINT = $(OBJDIR)/fcsc_AdminOperations.obj $(OBJDIR)/fcsc_service.obj
TST = $(OBJDIR)/test.obj
 
#all: $(APEXE2) $(APEXE1) #$(SWPRINT) 
all:  #$(APEXE1)  
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
	    $(CC) -o $(APEXE1) $(FCSCOMCLIENT) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(APEXE2): $(OBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    #$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(CC) -o $(APEXE2) $(FCSCSERVICE) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(APEXE3): $(OBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    #$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(CC) -o $(APEXE1) $(FCSCOMCLIENT) $(LIBSDIR) $(LIBS)
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

