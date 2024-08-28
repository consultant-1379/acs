/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2013
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file apos_ha_agent_types.h
 *
 * @brief
 *
 * This file defines all the enums, structures and macros
 * used by the other classes in AGENT.
 *
 * @author Shwetha N (xshweth)
 *
 ****************************************************************************/
#ifndef ACS_EMF_TYPES_H
#define ACS_EMF_TYPES_H


#include <string.h>

//==========================================================================
// constant delcaration
//==========================================================================
/* --*-- File Section --*-- */
#define ACS_EMF_FILE_NODE_ID	"/etc/cluster/nodes/this/id"
#define ACS_EMF_FILE_NODE_NAME	"/etc/cluster/nodes/this/hostname"
#define ACS_EMF_FILE_PEER_ID 		"/etc/cluster/nodes/peer/id"

/* --*-- Command Section --*-- */
//TR HU81933 dev/sdm changed in /dev/eri_thumbdrive for the next 3 defines
//#define ACS_EMF_ACTIVE_MOUNT_POINT	"/dev/sdm /data/opt/ap/internal_root/media ext3 rw,relatime,errors=continue,barrier=1,data=ordered 0 0"
#define ACS_EMF_ACTIVE_MOUNT_POINT	"/dev/eri_thumbdrive /data/opt/ap/internal_root/media ext3 rw,relatime,errors=continue,barrier=1,data=ordered 0 0"
//#define ACS_EMF_PASSIVE_MOUNT_POINT	"/dev/sdm /media ext3 rw,relatime,errors=continue,barrier=1,data=ordered 0 0"
#define ACS_EMF_PASSIVE_MOUNT_POINT	"/dev/eri_thumbdrive /media ext3 rw,relatime,errors=continue,barrier=1,data=ordered 0 0"
//#define ACS_EMF_THUMBDRIVE 			"/dev/sdm"
#define ACS_EMF_THUMBDRIVE 			"/dev/eri_thumbdrive"
#define ACS_EMF_ACTIVE_MEDIA_PATH	"/data/opt/ap/internal_root/media"
#define ACS_EMF_PASSIVE_MEDIA_PATH	"/media"
#define ACS_EMF_LOST_FOUND	"lost+found"
#define ACS_EMF_SLASH			"/"
#define ACS_EMF_ACTIVE_DATA	"/active_data"
#define ACS_EMF_CURRENT		"."
#define ACS_EMF_ASTERISK	"*"
#define ACS_EMF_CLUSTER		"/cluster"
#define ACS_EMF_FileM_PATH	"/data/opt/ap/internal_root"
#endif
