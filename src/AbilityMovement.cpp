#include "AbilityMovement.h"

MoveForwards::MoveForwards() : Ability()
{
	
}

void MoveForwards::Trigger()
{
	Parent()->SetVelocity(
		Parent()->getVelocity() + 
		(
			IRR.getRotatedVector(irr::core::vector3df(0,0,1), Parent()->getRotation()) *
			Parent()->mStats.GetThrust() *
			IRR.frameDeltaTime
		)
	);
}

MoveBackwards::MoveBackwards() : Ability()
{

}

void MoveBackwards::Trigger()
{
	Parent()->SetVelocity(
		Parent()->getVelocity() + 
		(
			IRR.getRotatedVector(irr::core::vector3df(0,0,-1), Parent()->getRotation()) *
			Parent()->mStats.GetThrust() *
			IRR.frameDeltaTime
		)
	);
}

StrafeLeft::StrafeLeft() : Ability()
{

}

void StrafeLeft::Trigger()
{
	Parent()->SetVelocity(
		Parent()->getVelocity() + 
		(
			IRR.getRotatedVector(irr::core::vector3df(-1,0,0), Parent()->getRotation()) *
			Parent()->mStats.GetStrafe() *
			IRR.frameDeltaTime
		)
	);
}

StrafeRight::StrafeRight() : Ability()
{

}

void StrafeRight::Trigger()
{
	Parent()->SetVelocity(
		Parent()->getVelocity() + 
		(
			IRR.getRotatedVector(irr::core::vector3df(1,0,0), Parent()->getRotation()) *
			Parent()->mStats.GetStrafe() *
			IRR.frameDeltaTime
		)
	);
}

PointAtTarget::PointAtTarget() : Ability()
{

}

void PointAtTarget::Trigger()
{
	float angle = Parent()->getAngleToVector(Parent()->getTargetPosition());

	Parent()->SetRotation(irr::core::vector3df(
		Parent()->getRotation().X,
		Parent()->getRotation().Y + (
			Parent()->mStats.GetTurnRate() *
			(angle / 1.f) *
			IRR.frameDeltaTime
		),
		Parent()->getRotation().Z
	));

	if(Parent()->getRotation().Y < -360.f)
	{
		Parent()->SetRotation(irr::core::vector3df(
			Parent()->getRotation().X,
			Parent()->getRotation().Y + 360.f,
			Parent()->getRotation().Z	
		));
	}
	else if(Parent()->getRotation().Y > 360.f)
	{
		Parent()->SetRotation(irr::core::vector3df(
			Parent()->getRotation().X,
			Parent()->getRotation().Y - 360.f,
			Parent()->getRotation().Z
		));
	}
}

