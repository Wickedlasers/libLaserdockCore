cmake_minimum_required(VERSION 3.11)

add_library(Box2D)

set_property(TARGET Box2D PROPERTY AUTOMOC OFF)
set_property(TARGET Box2D PROPERTY AUTORCC OFF)
set_property(TARGET Box2D PROPERTY AUTOUIC OFF)

target_include_directories(Box2D PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(Box2D PRIVATE

    ${SRCDIR}/Box2D/Box2D.h
    ${SRCDIR}/Box2D/Collision/b2BroadPhase.h
    ${SRCDIR}/Box2D/Collision/b2Collision.h
    ${SRCDIR}/Box2D/Collision/b2Distance.h
    ${SRCDIR}/Box2D/Collision/b2DynamicTree.h
    ${SRCDIR}/Box2D/Collision/b2TimeOfImpact.h
    ${SRCDIR}/Box2D/Common/b2BlockAllocator.h
    ${SRCDIR}/Box2D/Common/b2Draw.h
    ${SRCDIR}/Box2D/Common/b2GrowableStack.h
    ${SRCDIR}/Box2D/Common/b2Math.h
    ${SRCDIR}/Box2D/Common/b2Settings.h
    ${SRCDIR}/Box2D/Common/b2StackAllocator.h
    ${SRCDIR}/Box2D/Common/b2Timer.h
    ${SRCDIR}/Box2D/Dynamics/b2Body.h
    ${SRCDIR}/Box2D/Dynamics/b2ContactManager.h
    ${SRCDIR}/Box2D/Dynamics/b2Fixture.h
    ${SRCDIR}/Box2D/Dynamics/b2Island.h
    ${SRCDIR}/Box2D/Dynamics/b2TimeStep.h
    ${SRCDIR}/Box2D/Dynamics/b2World.h
    ${SRCDIR}/Box2D/Dynamics/b2WorldCallbacks.h
    ${SRCDIR}/Box2D/Rope/b2Rope.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2ChainShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2CircleShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2EdgeShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2PolygonShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2Shape.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2CircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2Contact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ContactSolver.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2DistanceJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2FrictionJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2GearJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2Joint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MotorJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MouseJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PulleyJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RopeJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WeldJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WheelJoint.h
    ${SRCDIR}/Box2D/Common/b2Math.h
    ${SRCDIR}/Box2D/Common/b2Settings.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2Shape.h
    ${SRCDIR}/Box2D/Common/b2BlockAllocator.h
    ${SRCDIR}/Box2D/Collision/b2Collision.h
    ${SRCDIR}/Box2D/Collision/b2DynamicTree.h
    ${SRCDIR}/Box2D/Common/b2GrowableStack.h
    ${SRCDIR}/Box2D/Collision/b2Distance.h
    ${SRCDIR}/Box2D/Dynamics/b2Body.h
    ${SRCDIR}/Box2D/Common/b2StackAllocator.h
    ${SRCDIR}/Box2D/Dynamics/b2ContactManager.h
    ${SRCDIR}/Box2D/Collision/b2BroadPhase.h
    ${SRCDIR}/Box2D/Dynamics/b2WorldCallbacks.h
    ${SRCDIR}/Box2D/Dynamics/b2TimeStep.h
    ${SRCDIR}/Box2D/Dynamics/b2Fixture.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2Joint.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2CircleShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2PolygonShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2EdgeShape.h
    ${SRCDIR}/Box2D/Collision/Shapes/b2ChainShape.h
    ${SRCDIR}/Box2D/Collision/b2TimeOfImpact.h
    ${SRCDIR}/Box2D/Common/b2Timer.h
    ${SRCDIR}/Box2D/Common/b2Draw.h
    ${SRCDIR}/Box2D/Dynamics/b2World.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2Contact.h
    ${SRCDIR}/Box2D/Dynamics/b2Island.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ContactSolver.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PulleyJoint.h
    ${SRCDIR}/Box2D/Rope/b2Rope.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2CircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2DistanceJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2FrictionJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2GearJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WheelJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MouseJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WeldJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RopeJoint.h
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MotorJoint.h

    ${SRCDIR}/Box2D/Collision/b2BroadPhase.cpp
    ${SRCDIR}/Box2D/Collision/b2CollideCircle.cpp
    ${SRCDIR}/Box2D/Collision/b2CollideEdge.cpp
    ${SRCDIR}/Box2D/Collision/b2CollidePolygon.cpp
    ${SRCDIR}/Box2D/Collision/b2Collision.cpp
    ${SRCDIR}/Box2D/Collision/b2Distance.cpp
    ${SRCDIR}/Box2D/Collision/b2DynamicTree.cpp
    ${SRCDIR}/Box2D/Collision/b2TimeOfImpact.cpp
    ${SRCDIR}/Box2D/Common/b2BlockAllocator.cpp
    ${SRCDIR}/Box2D/Common/b2Draw.cpp
    ${SRCDIR}/Box2D/Common/b2Math.cpp
    ${SRCDIR}/Box2D/Common/b2Settings.cpp
    ${SRCDIR}/Box2D/Common/b2StackAllocator.cpp
    ${SRCDIR}/Box2D/Common/b2Timer.cpp
    ${SRCDIR}/Box2D/Dynamics/b2Body.cpp
    ${SRCDIR}/Box2D/Dynamics/b2ContactManager.cpp
    ${SRCDIR}/Box2D/Dynamics/b2Fixture.cpp
    ${SRCDIR}/Box2D/Dynamics/b2Island.cpp
    ${SRCDIR}/Box2D/Dynamics/b2World.cpp
    ${SRCDIR}/Box2D/Dynamics/b2WorldCallbacks.cpp
    ${SRCDIR}/Box2D/Rope/b2Rope.cpp
    ${SRCDIR}/Box2D/Collision/Shapes/b2ChainShape.cpp
    ${SRCDIR}/Box2D/Collision/Shapes/b2CircleShape.cpp
    ${SRCDIR}/Box2D/Collision/Shapes/b2EdgeShape.cpp
    ${SRCDIR}/Box2D/Collision/Shapes/b2PolygonShape.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2CircleContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2Contact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2GearJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2Joint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MotorJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2MouseJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2RopeJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WeldJoint.cpp
    ${SRCDIR}/Box2D/Dynamics/Joints/b2WheelJoint.cpp
    )

add_library(${PROJECT_NAME}::Box2D ALIAS Box2D)
