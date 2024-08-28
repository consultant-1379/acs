/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImSearchableBlade.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-11-07  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImSearchableBlade.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImModelSaver.h"

TEST(ACS_CS_ImSearchableBlade, NoParametersSameReturnsTrue)
{
   // Arrange
   ACS_CS_ImSearchableBlade *search = new ACS_CS_ImSearchableBlade();
   ACS_CS_ImCpBlade *blade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   bool isSame = search->match(*blade);

   // Assert
   EXPECT_TRUE(isSame);

}

TEST(ACS_CS_ImSearchableBlade, SingleParameterMatchesReturnsTrue)
{
   // Arrange
   ACS_CS_ImSearchableBlade *search = new ACS_CS_ImSearchableBlade();
   search->slotNumber = 2;
   ACS_CS_ImCpBlade *blade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   bool isSame = search->match(*blade);

   // Assert
   EXPECT_TRUE(isSame);
}


TEST(ACS_CS_ImSearchableBlade, ThreeParameterMatchesReturnsOneTrueAndOneFalse)
{
   // Arrange
   ACS_CS_ImCpBlade *blade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImCpBlade *blade2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   blade2->systemNumber = 1234;

   ACS_CS_ImSearchableBlade *search = new ACS_CS_ImSearchableBlade();
   search->systemNumber = 1;
   search->systemNumberChanged = true;
   search->slotNumber = 2;
   search->slotNumberChanged = true;
   search->side = A;
   search->sideChanged = true;


   // Act
   bool isSame = search->match(*blade);   // Should match
   bool isSame2 = search->match(*blade2); // Should not match

   // Assert
   EXPECT_TRUE(isSame);
   EXPECT_FALSE(isSame2);
}


TEST(ACS_CS_ImSearchableBlade, MagazineAttributeMatchesCreatedShelf)
{
   // Arrange
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCpBlade *blade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   model->addObject(shelf);
   model->addObject(blade);

   ACS_CS_ImModelSaver saver(model);
   saver.save();

   // Act
   ACS_CS_ImSearchableBlade *search = new ACS_CS_ImSearchableBlade();
   search->magazine = "1.2.0.4";

   bool isSame = search->match(*blade);   // Should match

   shelf->action = ACS_CS_ImBase::DELETE;
   blade->action = ACS_CS_ImBase::DELETE;
   saver.save();

   EXPECT_TRUE(isSame);


}




