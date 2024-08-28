#ifndef ComMgmtSpiInterface_1_h
#define ComMgmtSpiInterface_1_h
#include <ComMgmtSpiCommon.h>

/**
 * The "base class" for all the component interfaces.
 *
 * @file ComMgmtSpiInterface_1.h
 *
 * The ComMgmtSpiInterface is the "base class" for all the component interfaces.
 * An interface to a component service interface has the following layout:
 *
 * @code
 * typedef struct SomeInterface_1
 * {
 *     ComMgmtSpiInterface_1T baseInterface;
 *     int (*aFunctionPointer)(int x, int y,...);
 *     ... more function pointers and stuff ...
 * } SomeInterface_1T;
 *
 * #define SomeInterface_1Id (ComMgmtSpiInterface_1T) { "SomeComponentName", "SomeInterfaceName", "1" }
 * @endcode
 *
 * The struct SomeInterface_1 with function pointers is an interface that
 * provides a part of or the whole service of a component.
 * The user of the interface will use the interface include file that the
 * provider supplies when programming with the interface.
 * The id definition is used by the service when instantiating and registering its
 * interface in the portal. Using the id definition is a convenient way
 * for the user of an interface to specify the in-argument when fetching
 * the interface from ComMgmtSpiInterfacePortal.
 *
 * The function pointers and the baseInterface members must be filled in before the
 * interface is registered in the COM Interface Portal. The base interface is best
 * filled in using the id definition.
 * See the example in ComMgmtSpiLibraryComponentManager_1.h.
 *
 * "1" denotes the incompatible C symbol changes, such as removing, adding
 * or changing the signature of the interface, and changes that affects the
 * behaviour of the interface in such a way that a user of the interface
 * must change its usage of the interface.
 *
 * The provider of an interface is responsible for supplying instances of
 * all versions of the interface. The members in ComMgmtSpiInterface struct is used by
 * ComMgmtSpiInterfacePortal since it does not know the C symbol of the particular
 * SomeService interface.
*/

/**
 * The "base class" for all the component interfaces.
 */
typedef struct ComMgmtSpiInterface_1 {
    /**
     * Name of the component.
     * The component is being registered under this name in the portal.
     */
    const char * componentName;
    /**
     * Name of the interface. The same as Interface excluding the version.
     * The component is implementing this interface.
     */
    const char * interfaceName;
    /**
     * Version of the interface.
     */
    const char * interfaceVersion;
} ComMgmtSpiInterface_1T;

#endif
