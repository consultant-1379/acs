
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

include $(COMMON_ROOT)/common.mk

SRCDIR = src
OBJDIR = obj
OUTDIR = .
#INCDIR = .
#INCINTDIR = inc_int
#DOCDIR = doc
#INCEXTDIR = inc_ext
#LIB_EXT=$(OUTDIR)/lib_ext
#LIB_INT=$(OUTDIR)/lib_int
#DEBUGDIR=$(OUTDIR)/.debug

#DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

#MODULE_NAME = CERTM_ADM
#MODULE_VERSION = 1_0
#MODULE_SEARCH = ./$(SRCDIR) ./inc
#export DOXYGEN_OUTPUT
#export MODULE_NAME
#export MODULE_VERSION
#export MODULE_SEARCH

# Setting application name
APP_FILENAME = timeout_client
APP_FILENAME_DBG = timeout_client_dbg

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/client_main.obj

## External Object files
MODULE_OBJS_EXT += 

DEPS := $(MODULE_OBJS:.obj=.d)

#ifneq ($(MAKECMDGOALS),clean)
#include $(DEPS)
#endif

## here you can add own lib paths
LIBSDIR += -L"../../../../dsd_cxc/bin/lib_ext"
#LIBSDIR += -L"$(AP_SDK_LIB)"
#LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/lib"
#LIBSDIR += -L"/vobs/IO_Developments/LOTC_SDK/lib64"

## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS +=

EXTRA_DEFINES +=

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"src"
CINCLUDES += -I"../../../$(INCEXTDIR)"
CINCLUDES += -I"../../../$(INCINTDIR)"
#CINCLUDES += -I"$(AP_SDK_INC)"
#CINCLUDES += -I"$(3PP_SDK_PATH)"

# here you can add own libs
LIBS += -lrt -lpthread
LIBS += -lacs_dsd
#LIBS += -lnetsnmp -lacs_trapdsapi
#LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbm.so.3.0.1
#LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbmpip.so.2.0.1
#IBS += -lacs_tra -llog4cplus

VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(APP_FILENAME)

$(OUTDIR)/$(APP_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APP_FILENAME) $(MODULE_OBJS) $(MODULE_OBJS_EXT) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SILENT)$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APP_FILENAME)
	$(RM) $(OUTDIR)/.debug/$(APP_FILENAME_DBG)

