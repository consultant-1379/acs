# **********************************************************************
#
# Short description:
# Makefile template for CS commands
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
#            2010-10-01   xmikhal   Updated with CS common.mk
#            2010-10-14   xminaon   Added hwcsync target
#            2010-10-18   xminaon   Moved definitions to common.mk
#			 2010-11-09   xmikhal   Added cpch target
#            2010-11-10   xpiokry   Updated to follow DR
#            2010-11-10   xmikhal   Added cpgls,cpgdef,cpgrm target
#            2010-11-12   xmikhal   Added fdls,fddef,fdrm target
#            2010-11-26   xpiokry   Added apzvlanls target
#            2011-01-12   xmaglex	Added targets: prebuild, postbuild and metrics        
# **********************************************************************

CNZ_NAME ?= cs_cnz
CAA_NAME ?= cscmd_caa
CXC_NAME ?= cs_cxc

include ../cs_cxc/include.mk

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

CAA_NAME = cscmd_caa
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
OUTDIR = ../cs_cxc/bin

# here you can add own lib paths
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) 
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(SEC_ACS_SDK_LIB)
LDFLAGS += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR)

LIBSDIR  += -L$(AP_SDK_LIB) 
LIBSDIR  += -L$(ACE_SDK_LIB)
LIBSDIR  += -L$(COREMW_SDK_LIB)
LIBSDIR  += -L$(BOOST_SDK_LIB)

CFLAGS += -fPIC -fmessage-length=0
CFLAGS += -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CFLAGS += 
	

# here you can add own Include paths and/or other includes
CINCLUDES += 	-I../csadm_caa/csprot/inc -I../csadm_caa/inc -I ../csapi_caa/inc_ext -I ../csapi_caa/inc_int \
		-I../csmodd_caa/inc/ -I../csadm_caa/common/inc -Icommon/inc -Iinc -I../csadm_caa/inc/imm_mapper 
					
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += -lrt -lpthread -lacs_csapi -lacs_apgcc -lacs_tra -lboost_serialization -lboost_system -lacs_dsd $(COREMW_LIBS)

# libcommon.a
LIBCOMMON_SRC = 	ACS_CS_Command_Exitcodes.cpp \
						ACS_CS_Attribute.cpp \
						acs_cs_moddcommands.cpp \
						ACS_CS_Common_Util.cpp	\
						ACS_CS_Command_Util.cpp
		 
LIBCOMMON_OBJ = $(LIBCOMMON_SRC:.cpp=.obj)

#STATIC_LIBS = $(OBJDIR)/libcommon.a $(LIBAPI_INT_DIR)/libprot.a
STATIC_LIBS = $(OBJDIR)/libcommon.a ../cs_cxc/bin/lib_int/libprot.a

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
CINCLUDES += -I../csadm_caa/csprot/inc
GCOV_FLAGS =
STATIC_LIBS += -lgcov
endif

# hwc commands
HWCLS_APNAME = hwcls
HWCLS_DEBUG = hwcls_dbg
HWCLS_APEXE = $(OUTDIR)/$(HWCLS_APNAME)
HWCLS_SRC = acs_cs_hwcls.cpp
HWCLS_OBJ = $(HWCLS_SRC:.cpp=.obj)

OPENSESSION_APNAME = opensession
OPENSESSION_DEBUG = opensession_dbg
OPENSESSION_APEXE = $(OUTDIR)/$(OPENSESSION_APNAME)
OPENSESSION_SRC = acs_cs_opensession.cpp
OPENSESSION_OBJ = $(OPENSESSION_SRC:.cpp=.obj)

#cpls
CPLS_APNAME = cpls
CPLS_DEBUG = cpls_dbg
CPLS_APEXE = $(OUTDIR)/$(CPLS_APNAME)
CPLS_SRC = acs_cs_cpls.cpp ACS_CS_Command_CPLS_Handler.cpp
CPLS_OBJ = $(CPLS_SRC:.cpp=.obj)

#cpgls
CPGLS_APNAME = cpgls
CPGLS_DEBUG = cpgls_dbg
CPGLS_APEXE = $(OUTDIR)/$(CPGLS_APNAME)
CPGLS_SRC = acs_cs_cpgls.cpp ACS_CS_Command_CPGLS_Handler.cpp
CPGLS_OBJ = $(CPGLS_SRC:.cpp=.obj)

vpath %.cpp common/src hwcls/src \
	../csadm_caa/test ../csadm_caa/src ../csadm_caa/common/src src
	
vpath %.h common/inc
vpath %.obj $(OBJDIR)

#Create all commands binaries
.NOTPARALLEL:
.PHONY: all
all: libcommon.a $(HWCLS_APNAME) $(OPENSESSION_APNAME) $(CPLS_APNAME) $(CPGLS_APNAME)

#Create common commands library
.PHONY: libcommon.a
libcommon.a: fs $(LIBCOMMON_OBJ)
	$(AR) cq $(OBJDIR)/$@ $(addprefix $(OBJDIR)/, $(LIBCOMMON_OBJ))

#Create hwcls command
.PHONY: $(HWCLS_APNAME)
$(HWCLS_APNAME): fs $(HWCLS_OBJ)
	$(NEW_LINE)
	$(SILENT)$(ECHO) "Linking $@..."
	$(CC) $(LDFLAGS) $(LIBSDIR) $(LIBS) $(addprefix $(OBJDIR)/,$(HWCLS_OBJ)) -o $(HWCLS_APEXE) $(STATIC_LIBS)
	$(call stripp,$(HWCLS_APNAME))
	$(NEW_LINE)

#Create opensession command
.PHONY: $(OPENSESSION_APNAME)
$(OPENSESSION_APNAME): fs $(OPENSESSION_OBJ)
	$(NEW_LINE)
	$(SILENT)$(ECHO) "Linking $@..."
	$(CC) $(LDFLAGS) $(LIBSDIR) $(LIBS) $(addprefix $(OBJDIR)/,$(OPENSESSION_OBJ)) -o $(OPENSESSION_APEXE) $(STATIC_LIBS)
	$(call stripp,$(OPENSESSION_APNAME))
	$(NEW_LINE)

#Create cpls command
.PHONY: $(CPLS_APNAME)
$(CPLS_APNAME): fs $(CPLS_OBJ)
	$(NEW_LINE)
	$(SILENT)$(ECHO) "Linking $@..."
	$(CC) $(LDFLAGS) $(LIBSDIR) $(LIBS) $(addprefix $(OBJDIR)/,$(CPLS_OBJ)) -o $(CPLS_APEXE) $(STATIC_LIBS)
	$(call stripp,$(CPLS_APNAME))
	$(NEW_LINE)

#Create cpgls command
.PHONY: $(CPGLS_APNAME)
$(CPGLS_APNAME): fs $(CPGLS_OBJ)
	$(NEW_LINE)
	$(SILENT)$(ECHO) "Linking $@..."
	$(CC) $(LDFLAGS) $(LIBSDIR) $(LIBS) $(addprefix $(OBJDIR)/,$(CPGLS_OBJ)) -o $(CPGLS_APEXE) $(STATIC_LIBS)
	$(call stripp,$(CPGLS_APNAME))
	$(NEW_LINE)
	
#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

#Clean object files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning CMD object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning CMD binary files: all commands'
	$(SILENT)$(RM) -f $(OUTDIR)/$(HWCLS_APNAME)
	$(SILENT)$(RM) -f $(OUTDIR)/$(OPENSESSION_APNAME)
	$(SILENT)$(RM) -f $(OUTDIR)/$(CPLS_APNAME)
	$(SILENT)$(RM) -f $(OUTDIR)/$(CPGLS_APNAME)
	
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

#Create dependencies
.PHONY: depend
depend: $(HWCLS_SRC)
	makedepend $(CINCLUDES) -o.obj $^

## DO NOT DELETE THIS LINE -- make depend needs it

