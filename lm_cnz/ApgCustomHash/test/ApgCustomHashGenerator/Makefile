CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

JAVACCOMPILE    = javac
OUTFLAG         = -d
CLASSPATHFLAG   = -cp
SOURCEPATHFLAG  = -sourcepath
BLOCKDIR        = $(ACS_ROOT)/lm_cnz/ApgCustomHash/test/ApgCustomHashGenerator
CLASSDIR        = $(BLOCKDIR)/bin
SRCDIRPARENT    = $(BLOCKDIR)/src
JARSDIR         = $(ACS_ROOT)/lm_cnz/ApgCustomHash/package
JAVASRCWILD     = *.java
DEPENDANTJARS   = $(JARSDIR)/ApgCustomHashImpl.jar
PACKAGEDIR      = $(BLOCKDIR)/package
ANTBUILD        = /app/APG43L/SDK/Tools/ant/bin/ant
ANTBUILDFLAG    = -buildfile
ANTBUILDPATH    = $(PACKAGEDIR)/ApgCustomHashCodeGenerator.xml
TARGETJARFILE   = $(PACKAGEDIR)/ApgCustomHashCodeGenerator.jar

.PHONY: all
all: CLASSCREATE JARCREATE

CLASSCREATE:
	$(JAVACCOMPILE) $(OUTFLAG) $(CLASSDIR) $(SOURCEPATHFLAG) $(SRCDIRPARENT) $(CLASSPATHFLAG) $(DEPENDANTJARS) $(SRCDIRPARENT)/$(JAVASRCWILD)

JARCREATE:
	$(call ct_co,$(TARGETJARFILE))
	$(ANTBUILD) $(ANTBUILDFLAG) $(ANTBUILDPATH)
	$(call ct_ci,$(TARGETJARFILE))

.PHONY:clean
clean:
	$(SILENT)$(ECHO) 'Cleaning file '
	$(SILENT) rm -f $(CLASSDIR)/*.class

