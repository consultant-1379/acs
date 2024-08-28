#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include <iostream>
#include <cstring>
#include <set>
#include <vector>

using namespace std;

TEST(Utils_printVlan, printsVlanButReturnsTrue)
{
   // Arrange
   ACS_CS_ImVlan *obj = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printVlanCategory, printsVlanCategoryButReturnsTrue)
{
   // Arrange
   ACS_CS_ImVlanCategory *obj = ACS_CS_ImDefaultObjectCreator::createVlanCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printShelfObject, printsShelfObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImShelf *obj = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printOtherBladeObject, printsOtherBladeObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printHardwareConfigurationCategoryObject, printsHardwareConfigurationCategoryObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImHardwareConfigurationCategory *obj = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printCpGroupCategoryObject, printsCpGroupCategoryObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printCpGroupObject, printsCpGroupObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImCpGroup *obj = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printCpCategoryObject, printsCpCategoryObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImCpCategory *obj = ACS_CS_ImDefaultObjectCreator::createCpCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printCpBladeObject, printsCpBladeObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImCpBlade *obj = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printCpObject, printsCpObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImCp *obj = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printConfigurationInfoObject, printsConfigurationInfoObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printBladeClusterInfoObject, printsBladeClusterInfoObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printApServiceCategoryObject, printsApServiceCategoryObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printApServiceObject, printsApServiceObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImApService *obj = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printApCategoryObject, printsApCategoryObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImApCategory *obj = ACS_CS_ImDefaultObjectCreator::createApCategoryObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printApBladeObject, printsApBladeObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImApBlade *obj = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printApObject, printsApObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImAp *obj = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

TEST(Utils_printAdvancedConfigurationObject, printsAdvancedConfigurationObjectButReturnsTrue)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImUtils::printImBaseObject(obj);

   // Assert
   ASSERT_TRUE(true);
}

