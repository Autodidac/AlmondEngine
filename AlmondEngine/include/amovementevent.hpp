/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#pragma once

#include "aplatform.hpp"      // Must always come first for platform defines
//#include "aengineconfig.hpp" // All ENGINE-specific includes

#include <iostream>

namespace almondnamespace
{
   class MovementEvent
   {
   public:
       MovementEvent(int entityId, float deltaX, float deltaY)
           : entityId(entityId), deltaX(deltaX), deltaY(deltaY)
       {
       }

       void print() const
       {
           std::cout << "Movement Event - Entity ID: " << entityId
               << ", Amount: (" << deltaX << ", " << deltaY << ")\n";
       }

       int getEntityId() const
       {
           return entityId;
       }
       float getDeltaX() const
       {
           return deltaX;
       }
       float getDeltaY() const
       {
           return deltaY;
       }

   private:
       int entityId; // ID of the entity to move
       float deltaX; // Change in X position
       float deltaY; // Change in Y position
   };
} // namespace almondnamespace