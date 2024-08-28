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
#      author: xsravan/zbhegna
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
   return LCTSMFCampaignPlugin()

class LCTSMFCampaignPlugin(SMFCampaignPlugin):
   # initialization method
   # self: variable represents the instance of the object itself
   def __init__(self):
      super(LCTSMFCampaignPlugin, self).__init__()
      self.MY_COMPONENT_UID = "acs.lct.lib"
      self._info = None
      self._utils = None
      self._actionType = None

   def prepare(self, csmModelInformationProvider, pluginUtilitiesProvider):
      # Here we save the references to the providers to be used later
      self._utils = pluginUtilitiesProvider
      self._info = csmModelInformationProvider

   def cliAtCampInit(self):
      self._actionType = self._info.getComponentActionType()
      result = []
      undoCli = ("/bin/true", None)
      if (self._info.getComponentActionType() == SMFConstants.CT_UPGRADE or self._info.getComponentActionType() == SMFConstants.CT_NOOP ):
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "apg_sw_control.sh" )
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)

      return result


#------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI actions that need to be executed during procedure wrapup phase
# These will be translated to a procWrapupAction doCLI in the generated campaign
# Second element in undoCli or doCli represents arguments used with doCliCommand or undoClicommand in generated campaign and is used as an argument for the script
# Third element in the tuple action is representing the Exec Environment, None represents SC-1,"safAmfNode=SC-2,safAmfCluster=myAmfCluster" represents SC-2
#------------------------------------------------------------------------------------------------------------------------------------------------------------
   def cliAtProcWrapup(self):
      result = []
      if self._info.getComponentActionType() == SMFConstants.CT_INSTALL:
         undoCli = ("/bin/true", None)

         cli = "/opt/ap/apos/conf/apos_postinstall.sh"
         doCli = (cli, None)
         action = (doCli,undoCli,None)
         result.append(action)
	 action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
	 result.append(action)

         cli = "/opt/ap/apos/conf/apos_password_hardenrules.sh"
         arg = "-d"
         doCli = (cli, arg)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)

         cli = "/opt/ap/acs/bin/acs_lct_configap.sh"
         arg = "-i"
         doCli = (cli, arg)
         action = (doCli,undoCli,None)
         result.append(action)

         arg = "-d"
         doCli = (cli, arg)
         action = (doCli,undoCli,None)
         result.append(action)

         cli = "/opt/ap/apos/conf/apos_finalize.sh"
         doCli = (cli, None)
         action = (doCli,undoCli,None)
         result.append(action)
	 action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
	 result.append(action)

         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "configure_tsuser.sh")
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)

         cli = "/opt/ap/apos/conf/apos_password_hardenrules.sh"
         arg = "-e"
         doCli = (cli, arg)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)


      return result


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
      if (self._actionType == SMFConstants.CT_UPGRADE or self._actionType == SMFConstants.CT_NOOP or self._actionType == SMFConstants.CT_INSTALL):
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "baseline_selector.sh")
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
      if self._info.getComponentActionType() == SMFConstants.CT_INSTALL:
         cli = "opt/ap/apos/conf/apos_ldapconf.sh"
         doCli = (cli, None)
         action = (doCli,undoCli,None)
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)

         cli = "/opt/ap/acs/bin/acs_lct_configap.sh"
         args = "-a"
         doCli = (cli, args)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,None)
         result.append(action)
      if self._actionType == SMFConstants.CT_UPGRADE:
         undoCli = ("/bin/true", None)
         cli = "/opt/ap/acs/bin/acs_lct_configap.sh"
         arg = "-i"
         doCli = (cli, arg)
         action = (doCli,undoCli,None)
         result.append(action)
      if self._actionType == SMFConstants.CT_UPGRADE:
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "add_cba_esi_group.sh")
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)
      if self._actionType == SMFConstants.CT_UPGRADE:
         cli = os.path.join("$OSAFCAMPAIGNROOT", self.MY_COMPONENT_UID, "scripts", "permissions.sh")
         doCli = (cli, None)
         action = (doCli,undoCli,"safAmfNode=SC-1,safAmfCluster=myAmfCluster")
         result.append(action)
         action = (doCli,undoCli,"safAmfNode=SC-2,safAmfCluster=myAmfCluster")
         result.append(action)

      return result
