#include "mouse.h"

Mouse::Mouse()
{
	mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
}

void Mouse::Update()
{
	mainMesh->setPosition(
		IntersectPlane(
			IRR.smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(
					IRR.receiver.MousePosition(),
					IRR.camera
				)
			)
	);
}

irr::core::vector3d<f32> Mouse::IntersectPlane(core::line3d<f32> myLine)
{
	irr::core::vector3d<f32>intersection(0, 0, 0);

	irr::core::plane3d<f32> plane(
		irr::core::vector3d<f32>(0, 32.f, 0),
		irr::core::vector3d<f32>(0, 1, 0)
	);

	plane.getIntersectionWithLine(myLine.start, myLine.end, intersection);

	return intersection;
}