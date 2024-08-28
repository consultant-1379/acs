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
#            2010-11-17   xgencol   Adjusted
# **********************************************************************

CNZ_NAME ?= aeh_cnz
CAA_NAME ?= aehapi_caa
CXC_NAME ?= aeh_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

LIBSDIR += -L$(CXC_PATH)/bin
LIBSDIR += -L"$(AP_SDK_LIB)"

# here you can add own linker flags
#LDFLAGS += -s

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(CAA_CMD_DIR)"
CINCLUDES += -I"$(CAA_API_DIR_INT)"
CINCLUDES += -I"$(CAA_API_DIR_EXT)"
CINCLUDES += -I"$(AP_SDK_INC)"


.phony: all clean distclean

AEHLS_OBJ = $(OBJDIR)/acs_aeh_aehls.obj

AEHEVLS_OBJ = $(OBJDIR)/acs_aeh_aehevls.obj

AEHLS_APNAME = aehls

AEHEVLS_APNAME = aehevls

AEHLS_APNAME_DBG = aehls_dbg

AEHEVLS_APNAME_DBG = aehevls_dbg

AEHLS_APEXE = $(OUTDIR)/$(AEHLS_APNAME)

AEHEVLS_APEXE = $(OUTDIR)/$(AEHEVLS_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(AEHLS_APNAME) \
	 $(OUTDIR)/$(AEHEVLS_APNAME)

$(OUTDIR)/$(AEHLS_APNAME): $(AEHLS_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(AEHLS_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(AEHLS_APEXE) $(AEHLS_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(AEHLS_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(AEHEVLS_APNAME): $(AEHEVLS_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(AEHEVLS_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(AEHEVLS_APEXE) $(AEHEVLS_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(AEHEVLS_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(AEHLS_APNAME)
	$(RM) -r $(OUTDIR)/$(AEHEVLS_APNAME)
