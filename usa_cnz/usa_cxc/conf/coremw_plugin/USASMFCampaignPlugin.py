#########################################################################
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
#      author: xnazbeg
#
#      Description:This script is used for the generation of SMF campaign
#
##########################################################################
from tcg.plugin_api.SMFCampaignPlugin import SMFCampaignPlugin
from tcg.plugin_api import SMFConstants
from tcg.plugin_api.SMFCampaignGenerationInfoProvider import SMFCampaignGenerationInfoProvider
from tcg.plugin_api.SMFPluginUtilitiesProvider import SMFPluginUtilitiesProvider
import os

def createSMFCampaignPlugin():
   return USASMFCampaignPlugin()

class USASMFCampaignPlugin(SMFCampaignPlugin):
   # initialization method
   # self: variable represents the instance of the object itself
   def __init__(self):
      super(USASMFCampaignPlugin, self).__init__()
      self.MY_COMPONENT_UID = "acs.usa"
      self._info = None
      self._utils = None

   def prepare(self, csmModelInformationProvider, pluginUtilitiesProvider):
      # Here we save the references to the providers to be used later
      self._utils = pluginUtilitiesProvider
      self._info = csmModelInformationProvider
#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during procedure Wrapup  phase
# These will be translated to a procWrapupAction doCLI,undoCli in the generated campaign
#------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtProcWrapup (self):
      script_arg = "default"
      return self.cliAtActions(script_arg)


   def cliAtActions(self,script_arg):
      self._actionType = self._info.getComponentActionType()
      result = []
      cli_script = "change_imm_attr.sh"
      if (self._actionType == SMFConstants.CT_UPGRADE):
         result.append(self.makeActions(cli_script,script_arg))
         return result
#----------------------------------------------------------------------------------------------------------------------------------------------------------
# makeActions method is to return CLI actions
# Second argumenet in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign
# Third argument in the tuple action is representing the Exec Environment, None represents SC-1
#------------------------------------------------------------------------------------------------------------------------------------------------------------
   def makeActions(self,script,args):
      undoCli = ("/bin/true",None)
      cli = os.path.join("$OSAFCAMPAIGNROOT",self.MY_COMPONENT_UID,"scripts",script)
      doCli = (cli,args)
      action = (doCli,undoCli,None)
      return action

#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during campaign complete phase
# These will be translated to a campCompleteAction doCLI in the generated campaign
# Second element in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign and is used as an argument for the script
# Third element in the tuple action is representing the Exec Environment, None represents SC-1,"safAmfNode=SC-2,safAmfCluster=myAmfCluster" represents SC-2
#-------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtCampComplete(self):
      self._actionType = self._info.getComponentActionType()
      result = []
      undoCli = ("/bin/true", None)
      if self._info.getComponentActionType() == SMFConstants.CT_UPGRADE:
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "change_imm_attr.sh")
         args = "delTmp"
         doCli = (cli, args)
	 action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)
      return result
