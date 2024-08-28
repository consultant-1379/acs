
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

SRCDIR = src
OBJDIR = obj
OUTDIR = .

# Setting application name
APP_FILENAME = acs_apbmd
APP_FILENAME_DBG = acs_apbmd_dbg

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/acs_apbm_haappmanager_t.obj \
	$(OBJDIR)/acs_apbm_logger_t.obj \
	$(OBJDIR)/acs_apbm_main_t.obj \
	$(OBJDIR)/acs_apbm_serverwork_t.obj

## External Object files
MODULE_OBJS_EXT += ../../obj/acs_apbm_alarmeventhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_cmdoptionparser.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_csreader.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_programconfiguration.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_action_timer.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_boardserviceacceptor.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_boardsessionhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_hwidata.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_hwihandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_hwiimmconfobjhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_hwiimmruntimeobjhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_immrepositoryhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_ipmiupgradehandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_operationpipescheduler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_sanotifiermanager.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_serverworkingset.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_shelvesdatamanager.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmpmanager.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmpmanagercallbacks.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmpsessioncallback.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmpsessionhandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmpsessioninfo.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_trapsubscriptionmanager.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_cshwctablechangeobserver.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_snmptrapdsobserver.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_snmptrapmessagehandler.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_procsignalseventhandler.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_monitoringservicehandler.obj
MODULE_OBJS_EXT += ../../obj/acs_apbm_operationpipeeventhandler.obj

MODULE_OBJS_EXT += ../../obj/acs_apbm_sanotifiereventhandler.obj


DEPS := $(MODULE_OBJS:.obj=.d)

#ifneq ($(MAKECMDGOALS),clean)
#include $(DEPS)
#endif

## here you can add own lib paths
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/LATEST/log4cplus/lib/"
LIBSDIR += -L"$(LOTC_SDK_LIB)"


## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS +=

EXTRA_DEFINES +=
EXTRA_DEFINES += -DACS_APBM_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DACS_APBM_LOGGING_USE_PRETTY_FUNCTION

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"src"
CINCLUDES += -I"../../inc"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"


# here you can add own libs
LIBS += -lrt -lpthread -lnetsnmp -lacs_trapdsapi
LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbm.so.3.3.1
LIBS += $(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext/libacs_apbmpip.so.2.2.2
LIBS += -lacs_tra -llog4cplus

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

.PHONY: prebuild
prebuild:
	$(call ct_co,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_co,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_ci,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_unco,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APP_FILENAME)
	$(RM) $(OUTDIR)/.debug/$(APP_FILENAME_DBG)

