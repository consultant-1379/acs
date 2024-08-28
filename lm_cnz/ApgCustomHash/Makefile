CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

JAVACCOMPILE 	= javac
OUTFLAG 	= -d
CLASSPATHFLAG 	= -cp
SOURCEPATHFLAG 	= -sourcepath
BLOCKDIR 	= $(ACS_ROOT)/lm_cnz/ApgCustomHash
CLASSDIR  	= $(BLOCKDIR)/bin
CLASSPACKAGEERIC= $(CLASSDIR)/com/ericsson/apg/keygen
CLASSPACKAGESAFE= $(CLASSDIR)/com/safenet/poetic/keygen
SRCDIRPARENT    = $(BLOCKDIR)/src
ERICSSONSRCDIR 	= $(BLOCKDIR)/src/com/ericsson/apg/keygen
SAFENETSRCDIR 	= $(BLOCKDIR)/src/com/safenet/poetic/keygen
JARSDIR   	= $(BLOCKDIR)/lib
JAVASRCWILD 	= *.java
DEPENDANTJARS 	= $(JARSDIR)/CaffeUtil.jar:$(JARSDIR)/Platform.jar:$(JARSDIR)/javolution.jar:$(JARSDIR)/jna.jar:$(JARSDIR)/license-core.1.8.0.jar:$(JARSDIR)/license-decode.1.8.0.jar:$(JARSDIR)/license-initialization.1.8.0.jar
PACKAGEDIR 	= $(BLOCKDIR)/package
ANTBUILD 	= /app/APG43L/SDK/Tools/ant/bin/ant
ANTBUILDFLAG 	= -buildfile
ANTBUILDPATH 	= $(PACKAGEDIR)/ApgCustomHash.xml
TARGETJARFILE   = $(PACKAGEDIR)/ApgCustomHashImpl.jar

.PHONY: all
all: CLASSCREATE JARCREATE 

CLASSCREATE:
	$(JAVACCOMPILE) $(OUTFLAG) $(CLASSDIR) $(SOURCEPATHFLAG) $(SRCDIRPARENT) $(CLASSPATHFLAG) $(DEPENDANTJARS) $(ERICSSONSRCDIR)/$(JAVASRCWILD) $(SAFENETSRCDIR)/$(JAVASRCWILD)

JARCREATE:
	$(call ct_co,$(TARGETJARFILE))
	$(ANTBUILD) $(ANTBUILDFLAG) $(ANTBUILDPATH)
	$(call ct_ci,$(TARGETJARFILE))

.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning file '
	$(SILENT) rm -f $(CLASSPACKAGEERIC)/*.class
	$(SILENT) rm -f $(CLASSPACKAGESAFE)/*.class

