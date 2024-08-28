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
#      author: zbhegna
# Changelog:
# - Wed Oct 32 2086 - Gnaneswara Seshu (zbhegna) 
#       TR HX29554 fix
#
#      Description:This script is used for the generation of SMF campaign
#
###############################################################
from tcg.plugin_api.SMFCampaignPlugin import SMFCampaignPlugin
from tcg.plugin_api import SMFConstants
from tcg.plugin_api.SMFCampaignGenerationInfoProvider import SMFCampaignGenerationInfoProvider
from tcg.plugin_api.SMFPluginUtilitiesProvider import SMFPluginUtilitiesProvider
import os

def createSMFCampaignPlugin():
   return PRCSMFCampaignPlugin()

class PRCSMFCampaignPlugin(SMFCampaignPlugin):
   # initialization method
   # self: variable represents the instance of the object itself
   def __init__(self):
      super(PRCSMFCampaignPlugin, self).__init__()
      self.MY_COMPONENT_UID = "acs.prc"
      self._info = None
      self._utils = None
      self._actionType = None

   def prepare(self, csmModelInformationProvider, pluginUtilitiesProvider):
      # Here we save the references to the providers to be used later
      self._utils = pluginUtilitiesProvider
      self._info = csmModelInformationProvider
#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during campaign initiation phase
# These will be translated to a campCompleteAction doCLI in the generated campaign
# Second argument in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign
# Third argument in the tuple action is representing the Exec Environment, None represents SC-1
#---------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtCampInit (self):
      self._actionType = self._info.getComponentActionType()
      result = []
      undoCli = ("/bin/true", None)
      if (self._actionType == SMFConstants.CT_UPGRADE or self._actionType == SMFConstants.CT_NOOP or self._actionType == SMFConstants.CT_MIGRATE):
         cli = "opt/ap/acs/bin/cyclic_reboot_delay_config.sh"
         args = "-r"
         doCli = (cli, args)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)

      return result

   def cliAtCampComplete(self):
      self._actionType = self._info.getComponentActionType()
      result = []
      undoCli = ("/bin/true", None)
      if (self._actionType == SMFConstants.CT_MIGRATE):
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "prc_restart.sh")
         args = None
         doCli = (cli, args)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)

      return result

