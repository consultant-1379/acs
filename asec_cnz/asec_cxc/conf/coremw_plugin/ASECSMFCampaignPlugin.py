###############################################################
#
#      COPYRIGHT Ericsson 2019
#      All rights reserved.
#
#      The Copyright to the computer program(s) herein
#      is the property of Ericsson 2019.
#      The program(s) may be used and/or copied only with
#      the written permission from Ericsson 2019 or in
#      accordance with the terms and conditions stipulated in
#      the agreement/contract under which the program(s) have
#      been supplied.
#
# Changelog:
# -  17 April 2019 - Sowjanya Medak (xsowmed)
#       First version
#
###############################################################
from tcg.plugin_api.SMFCampaignPlugin import SMFCampaignPlugin
from tcg.plugin_api import SMFConstants
from tcg.plugin_api.SMFCampaignGenerationInfoProvider import SMFCampaignGenerationInfoProvider
from tcg.plugin_api.SMFPluginUtilitiesProvider import SMFPluginUtilitiesProvider
import os

def createSMFCampaignPlugin():
   return ASECSMFCampaignPlugin()

class ASECSMFCampaignPlugin(SMFCampaignPlugin):
   # initialization method
   # self: variable represents the instance of the object itself
   def __init__(self):
      super(ASECSMFCampaignPlugin, self).__init__()
      self.MY_COMPONENT_UID = "acs.asec"
      self._info = None
      self._utils = None
      self._actionType = None

   def prepare(self, csmModelInformationProvider, pluginUtilitiesProvider):
      # Here we save the references to the providers to be used later
      self._utils = pluginUtilitiesProvider
      self._info = csmModelInformationProvider

#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during campaign complete phase
# These will be translated to a campCompleteAction doCLI in the generated campaign
# Second argument in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign
# Third argument in the tuple action is representing the Exec Environment, None represents SC-1,"safAmfNode=SC-2,safAmfCluster=myAmfCluster" represents SC-2
#---------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtCampComplete(self):
      self._actionType = self._info.getComponentActionType()
      result = []
      undoCli = ("/bin/true", None)
      if self._info.getComponentActionType() == SMFConstants.CT_UPGRADE:
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "acs_csadm_custom_networks_fix.sh")
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
      return result


