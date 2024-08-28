#ifndef COM_CLI_API_TUNNEL_1_H
#define COM_CLI_API_TUNNEL_1_H

#include "ComMgmtSpiCommon.h"
#include "ComMgmtSpiInterface_1.h"

#include <stdint.h>

/**
 *
 * @file ComCliApiTunnel_1.h
 * Interface for CLI tunneling.
 */

/**
 * Identity for the interface
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComCliApiCmdTunnelIf_1Id = { "ComCliAgent", "ComCliApiTunnel", "1" };
#endif
#ifndef __cplusplus
#define ComCliApiCmdTunnelIf_1Id (ComMgmtSpiInterface_1T){"ComCliAgent", "ComCliApiTunnel", "1"}
#endif

/**
 * Type to store tunnel handle specific id.
 */
typedef uint64_t  ComCliTunnelHandleT;

/**
  * callback provided by the user of the interface to handle entered text from the user
  * @param userHandle handle defined by the user a start tunnel. This may be used to lookup user data
  *  associated to the tunnel instance.
  * @param text input entered by the user
  */
typedef void  (*ComCliWriteTunnelCallback)(ComCliTunnelHandleT userHandle, const char* text);


/**
 * callback provided by the user of the interface to handle that the tunnel is closed by initiative from the implementor of this interface (COM)
 * @param userHandle handle defined by the user a start tunnel. This may be used to lookup user data
 *  associated to the tunnel instance.
 */
typedef void  (*ComCliStopTunnelCallback)(ComCliTunnelHandleT userHandle);


/**
 * callback invoked for setting the window size for a the tunnel end.
 * @param userHandle a handle defined by the caller with will passed back in the callbacks
 * @param ncolumn the number of column in the window
 * @param nline the number of lines in the window
 */
typedef void  (*ComCliWindowSizeCallback)(ComCliTunnelHandleT userHandle, int ncolumn, int nline);


/**
 *  Interface for managing a CLI tunnel. The tunnel is a transparent channel through the COM CLI subsystem (CLI agent and the CLI subshell).
 *  The point in this is to make it possible to open a subshell from the CLI.
 */
typedef struct ComCliApiTunnel_1 {

    /**
      * Common interface description.
      * The "base class" for this interface contains the component
      * name, interface name, and interface version.
      */
    ComMgmtSpiInterface_1T base;



    /**
     * function pointer used by a command component to ask CliAgent to start the tunneling mode
     * @param userHandle a handle defined by the caller with will passed back in the callbacks
     * @param sessionId the session ID for the CLI session that shall start tunnel mode
     * @param writeFunction callback function (provided by the caller of this method) for handling of data entered by the operator
     * @param stopFunction callback function for handling that COM stops the tunnel.
     * @param windowSizeFunction callback invoked when the hosting window has changed size.
     * The callback will also be invoked after the the tunnel is estatblished to get the initial window size.
     * This parameter is optional and may be NULL if not used.
     * @return the status of the operation
     */
    ComReturnT (*startTunnel)(ComCliTunnelHandleT userHandle,
                              uint32_t sessionId,
                              ComCliWriteTunnelCallback writeFunction,
                              ComCliStopTunnelCallback  stopFunction,
                              ComCliWindowSizeCallback windowSizeFunction);


    /**
     * function pointer used by a command component to ask CliAgent to end the tunneling mode
     * @param sessionId the session ID for the CLI session that shall end tunnel mode
     * @return the status of the operation
     */
    ComReturnT (*endTunnel)(uint32_t sessionId);


    /**
     * function pointer used by a command component to print output from the tunnel (back to the operator)
     * @param sessionId the session ID for the CLI session (that must be in tunnel mode)
     * @param text the text to print out. Nothing is added (such as CR/LF).
     * @return the status of the operation
     */
    ComReturnT (*printOutput)(uint32_t sessionId,
                              const char *text);


} ComCliApiTunnel_1T;

#endif



