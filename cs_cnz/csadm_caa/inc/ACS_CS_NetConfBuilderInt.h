
/*

	  CLASS DIAGRAM ACS_CS_NetConf<x>
 
 
      +-----------+                  +------------+                +----------+
      | SAXParser |                  |ConcCommand |                |SenderInt |
      |-----------|                  |------------|                |----------|<-------+
      |           |    +-----------<>+            +<>--------------+          |        |
      |           |    |             |            |                |          |        |
      +----+^-----+    |             +            +------------+   +-----^----+        |
            |          |   +------+  |            |            |         |             |
            |          |   |XStr  |  +---+--------+            |         |             |
      +-----+-----+    |   --------      |                     |    +----+-----+  +----+-------+
      | ParserInt +----+   |      |      |                     |    |LocalSender  |RemoteSender|
      |-----------|        +------+      |                     |    |----------|  |------------|
      |           |    inner  |          |                     |    |          |  |            |
      |           |       +---+-----+    |      +----------+   |    |          |  |            |
      |           |       |FilterInt|    |      |CommandInt|   |    |          |  |            |
      +-----^-----+       |---------|    |      |----------|   |    +----------+  +------------+
            |             |         |    +------>          |   |          |             |
            |             |         |           |          |   |          |             |
      +-----|-----+       +----^----+           |          |   |          |             |
      |ConcrParser|            |                +----------+   |          |             |
      |-----------|       +----+----+-                         |          |             |
      |           |       |ConcFilter|                         +------+   |             |
      |           |       |----------|                                |   |             |
      |           |       |          |                                |   |             |
      +-----------+       |          |                                |   |             |
            |             +----------+                                |   |             |
            |                                                         |   |             |
            |                                     +-----------+       |   |             |
            |                                     |Builder    |<>-----+   |             |
            |                                     |-----------|           |             |
            |                                     |makeCommand|<>---------+             |
            |                                     |           |<>-----------------------+
            +-----------------------------------<>|           |
                                                  +-----------+
                                                        |
                                                        |
                                                  +-----v-----+
                                                  |BuilderInt |
                                                  |-----------|
                                                  |makeCommand|
                                                  +-----------+

 */


#ifndef ACS_CS_NETCONF_BUILDER_INT_H
#define	ACS_CS_NETCONF_BUILDER_INT_H

#include "ACS_CS_NetConfCommandInt.h"


namespace netconfBuilder
{
	enum builds {
		BLADE=1,
		HARDWARE=2,
		UDP=3
	};
}

class ACS_CS_Netconf_Builder_int {
public:
    
    ACS_CS_Netconf_Builder_int()
    {
        
    }
    
    virtual ~ACS_CS_Netconf_Builder_int()
    {
        
    }
    virtual ACS_CS_NetConfCommandInt* make(netconfBuilder::builds g)=0;
    
private:

};


#endif	/* ACS_CS_NETCONF_BUILDER_INT_H */

