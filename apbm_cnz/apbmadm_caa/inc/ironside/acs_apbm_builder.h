
/*

	  CLASS DIAGRAM acs_apbm_ironside<x>
 
 
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


#ifndef ACS_APBM_IRONSIDE_BUILDER_INT_H
#define	ACS_APBM_IRONSIDE_BUILDER_INT_H

#include "acs_apbm_command.h"


namespace ironsideBuilder
{
	enum builds {
		UDP             = 4
};

namespace PHYSICAL_POSITION
{
	struct query{
		char magazine[10];
		int slot;

	};
}


}

class acs_apbm_builder {
public:
    
	acs_apbm_builder()
    {
        
    }
    
    virtual ~acs_apbm_builder()
    {
        
    }
    virtual acs_apbm_command* make(ironsideBuilder::builds g, const unsigned* = 0) = 0;
    
private:

};


#endif	/* ACS_CS_IRONSIDE_BUILDER_INT_H */

