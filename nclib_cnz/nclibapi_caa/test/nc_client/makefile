include $(COMMON_ROOT)/common.mk

SRCDIR = src
OBJDIR = obj
OUTDIR = .
INCDIR = ../../inc_ext
#INCINTDIR = inc_int
#DOCDIR = doc
#INCEXTDIR = inc_ext
#LIB_EXT=$(OUTDIR)/lib_ext
#LIB_INT=$(OUTDIR)/lib_int
#DEBUGDIR=$(OUTDIR)/.debug

# Setting application name
APP_FILENAME = nc_client
APP_FILENAME_ACTION = nc_client_action
APP_FILENAME_DBG = nc_client_dbg
APP_FILENAME_ACTION_DBG = nc_client_action_dbg

## Object files
MODULE_OBJS += $(OBJDIR)/nc_client.obj
MODULE_OBJS_ACTION += $(OBJDIR)/nc_client_action.obj

## External Object files
MODULE_OBJS_EXT +=
#MODULE_OBJS_EXT += ../../obj/acs_apbm_alarmeventhandler.obj
#MODULE_OBJS_EXT += ../../obj/acs_apbm_cmdoptionparser.obj
#MODULE_OBJS_EXT += ../../obj/acs_apbm_csreader.obj
#MODULE_OBJS_EXT += ../../obj/acs_apbm_programconfiguration.obj

DEPS := $(MODULE_OBJS:.obj=.d)

#ifneq ($(MAKECMDGOALS),clean)
#include $(DEPS)
#endif

## here you can add own lib paths
LIBSDIR += -L"../../../nclib_cxc/bin/lib_ext"
#LIBSDIR += -L"$(3PP_SDK_PATH)/libssh2/lib64"
#LIBSDIR += -L"$(AP_SDK_LIB)"
#LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/lib"
#LIBSDIR += -L"/vobs/IO_Developments/LOTC_SDK/lib64"

## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS +=

EXTRA_DEFINES +=
#EXTRA_DEFINES += -DACS_APBM_LOGGING_HAS_SOURCE_INFO
#EXTRA_DEFINES += -DACS_APBM_LOGGING_USE_PRETTY_FUNCTION

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"src"
CINCLUDES += -I"../../inc_ext"
CINCLUDES += -I"../../inc_int"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"

# here you can add own libs
LIBS += -lrt -lpthread -lacs_nclib 
#LIBS += -lnetsnmp -lacs_trapdsapi
#LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbm.so.3.0.1
#LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbmpip.so.2.0.1
#LIBS += -lacs_tra -llog4cplus

VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: client action

.PHONY: client CFLAGS += $(GCOV_FLAGS)
client: $(OUTDIR)/$(APP_FILENAME)

$(OUTDIR)/$(APP_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APP_FILENAME) $(MODULE_OBJS) $(MODULE_OBJS_EXT) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SILENT)$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
.PHONY: action CFLAGS += $(GCOV_FLAGS)
action: $(OUTDIR)/$(APP_FILENAME_ACTION)

$(OUTDIR)/$(APP_FILENAME_ACTION): $(MODULE_OBJS_ACTION)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME_ACTION)'	
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APP_FILENAME_ACTION) $(MODULE_OBJS_ACTION) $(MODULE_OBJS_EXT) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SILENT)$(call stripp,$(APP_FILENAME_ACTION))
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
	$(RM) -r $(OUTDIR)/$(APP_FILENAME_ACTION)
	$(RM) $(OUTDIR)/.debug/$(APP_FILENAME_ACTION_DBG)

