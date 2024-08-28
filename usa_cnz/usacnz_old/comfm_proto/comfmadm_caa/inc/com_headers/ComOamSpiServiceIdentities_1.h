#ifndef ComOamSpiServicesId
#define ComOamSpiServicesId

/**
 * Identities definition for the operation and maintenance services.
 *
 * @file ComOamSpiServiceIdentities_1.h
 *
 * This file specifies the interface identities used when fetching
 * the interface from the ComMgmtSpiInterfacePortal.
 *
 * Considering usability reasons, the version of this file might not be
 * incremented as long as the change introduced remains backwards compatible.
 */
#include <ComMgmtSpiInterface_1.h>

/**
 * Identity of the Managed Object SPI implemented by the Ext CM Router service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiExtCmRouterService_1Id = {"ComOamSpiExtCmRouterService", "ComOamSpiManagedObject", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiExtCmRouterService_1Id (ComMgmtSpiInterface_1T){"ComOamSpiExtCmRouterService", "ComOamSpiManagedObject", "1"}
#endif

/**
 * Identity of the Managed Object SPI implemented by the CM Router service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiCmRouterService_1Id = {"ComOamSpiCmRouterService", "ComOamSpiManagedObject", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiCmRouterService_1Id (ComMgmtSpiInterface_1T){"ComOamSpiCmRouterService", "ComOamSpiManagedObject", "1"}
#endif

/**
 * Identity of the Model Repository SPI implemented by the Model Repository service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiModelRepository_1Id = {"ComOamSpiModelRepositoryService", "ComOamSpiModelRepository", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiModelRepository_1Id (ComMgmtSpiInterface_1T){"ComOamSpiModelRepositoryService", "ComOamSpiModelRepository", "1"}
#endif

/**
 * Identity of the Model Repository SPI II implemented by the Model Repository service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiModelRepository_2Id = {"ComOamSpiModelRepositoryService", "ComOamSpiModelRepository", "2"};
#endif
#ifndef __cplusplus
#define ComOamSpiModelRepository_2Id (ComMgmtSpiInterface_1T){"ComOamSpiModelRepositoryService", "ComOamSpiModelRepository", "2"}
#endif

/**
 * Identity of the Transaction SPI implemented by the Transaction service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiTransaction_1Id = {"ComOamSpiTransactionService", "ComOamSpiTransaction", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiTransaction_1Id (ComMgmtSpiInterface_1T){"ComOamSpiTransactionService", "ComOamSpiTransaction", "1"}
#endif

/**
 * Identity of the Transaction Master SPI implemented by the Transaction service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiTransactionMaster_1Id = {"ComOamSpiTransactionService", "ComOamSpiTransactionMaster", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiTransactionMaster_1Id (ComMgmtSpiInterface_1T){"ComOamSpiTransactionService", "ComOamSpiTransactionMaster", "1"}
#endif

/**
 * Identity of the Transaction Resource SPI implemented by any service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiTransactionalResource_1Id = {0, "ComOamSpiTransactionalResource", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiTransactionalResource_1Id (ComMgmtSpiInterface_1T){NULL, "ComOamSpiTransactionalResource", "1"}
#endif

/**
 * Identity of the Notification Producer SPI implemented by any service.
 */
#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiNotificationProducer_1Id = { 0, "ComOamSpiNotificationProducer", "1" };
#endif
#ifndef __cplusplus
#define ComOamSpiNotificationProducer_1Id (ComMgmtSpiInterface_1T){NULL, "ComOamSpiNotificationProducer", "1"}
#endif


/**
 * Identity of the Event service SPI
 */

#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiEventService_1Id = {"ComOamSpiEventService", "ComOamSpiEventRouter", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiEventService_1Id (ComMgmtSpiInterface_1T){"ComOamSpiEventService", "ComOamSpiEventRouter", "1"}
#endif

/**
 * Identity of the Security Management SPI
 */

#ifdef __cplusplus
const ComMgmtSpiInterface_1T ComOamSpiSecurityManagement_1Id = {"ComOamSpiSecurityManagementService", "ComOamSpiSecurityManagement", "1"};
#endif
#ifndef __cplusplus
#define ComOamSpiSecurityManagement_1Id (ComMgmtSpiInterface_1T){"ComOamSpiSecurityManagementService", "ComOamSpiSecurityManagement", "1"}
#endif

#endif
