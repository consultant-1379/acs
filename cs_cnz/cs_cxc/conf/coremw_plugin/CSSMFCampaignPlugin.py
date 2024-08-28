###############################################################
#
#      COPYRIGHT Ericsson 2018
#      All rights reserved.
#
#      The Copyright to the computer program(s) herein
#      is the property of Ericsson 2018.
#      The program(s) may be used and/or copied only with
#      the written permission from Ericsson 2018 or in
#      accordance with the terms and conditions stipulated in
#      the agreement/contract under which the program(s) have
#      been supplied.
#
# Changelog:
# -  24 Jul 2019 - Sankara Jayanth (xsansud)
#    First version
#
###############################################################
from tcg.plugin_api.SMFCampaignPlugin import SMFCampaignPlugin
from tcg.plugin_api import SMFConstants
from tcg.plugin_api.SMFCampaignGenerationInfoProvider import SMFCampaignGenerationInfoProvider
from tcg.plugin_api.SMFPluginUtilitiesProvider import SMFPluginUtilitiesProvider
import os

def createSMFCampaignPlugin():
   return CSSMFCampaignPlugin()

class CSSMFCampaignPlugin(SMFCampaignPlugin):
   # initialization method
   # self: variable represents the instance of the object itself
   def __init__(self):
      super(CSSMFCampaignPlugin, self).__init__()
      self.MY_COMPONENT_UID = "acs.cs"
      self._info = None
      self._utils = None
      self._actionType = None

   def prepare(self, csmModelInformationProvider, pluginUtilitiesProvider):
      # Here we save the references to the providers to be used later
      self._utils = pluginUtilitiesProvider
      self._info = csmModelInformationProvider

#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during proc init phase
# These will be translated to a ProcInitAction doCLI in the generated campaign
# Second argument in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign
# Third argument in the tuple action is representing the Exec Environment
#-------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtProcInit(self):
      self._actionType = self._info.getComponentActionType()
      cs = self._info.getComponent("acs.cs")
      for bundle in cs.swBundles:
         bundleName = bundle[1] 
      result = []
      undoCli = ("/bin/true", None)
      if (self._actionType == SMFConstants.CT_INSTALL): 
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "cs_helper.sh")
         args = "init"+" "+bundleName
         doCli = (cli, args)
         action = (doCli,undoCli,None)
         result.append(action)
      if (self._actionType == SMFConstants.CT_UPGRADE or self._actionType == SMFConstants.CT_INSTALL):
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "cs_transport_helper.sh" )
         contextStr = ""
         if(self._actionType == SMFConstants.CT_UPGRADE):
            contextStr = "init_upgrade"
         if(self._actionType == SMFConstants.CT_INSTALL):
            contextStr = "init_install"
         args=contextStr+" "+bundleName
         doCli = (cli, args)
         action =(doCli,undoCli,None)
         result.append(action)

      return result
 
#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during procWrapup phase
# These will be translated to a procWrapupAction doCLI in the generated campaign
# Second argument in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign
# Third argument in the tuple action is representing the Exec Environment, None represents SC-1,"safAmfNode=SC-2,safAmfCluster=myAmfCluster" represents SC-2
#---------------------------------------------------------------------------------------------------------------------------------------------------------------    
   def cliAtProcWrapup(self):
      self._actionType = self._info.getComponentActionType()
      cs = self._info.getComponent("acs.cs")
      for bundle in cs.swBundles:
         bundleName = bundle[1]
      result = []
      undoCli = ("/bin/true", None)
      if (self._actionType == SMFConstants.CT_INSTALL):
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "cs_helper.sh")
         args = "wrapup"+" "+bundleName
         doCli = (cli, args)
         action = (doCli,undoCli,None)
         result.append(action)
      return result
