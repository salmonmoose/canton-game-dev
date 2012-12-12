#include "mouse.h"

Mouse::Mouse()
{
	mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
    Position = irr::core::vector3df(0.f,0.f,0.f);
}

void Mouse::Update(float height)
{
    irr::core::line3df mouseLine = getRay();

    Position = IntersectPlane(mouseLine, height);

	//mainMesh->setPosition(Position); //should align with player height.
}

irr::core::vector3df Mouse::getPosition()
{
    return Position;
}

irr::core::line3df Mouse::getRay()
{
    irr::core::line3d<f32> ln(0,0,0,0,0,0);
    const scene::SViewFrustum* f = IRR.camera->getViewFrustum();

    irr::core::vector3df farLeftUp = f->getFarLeftUp();
    irr::core::vector3df lefttoright = f->getFarRightUp() - farLeftUp;
    irr::core::vector3df uptodown = f->getFarLeftDown() - farLeftUp;

    irr::core::dimension2d<u32> screenSize(IRR.driver->getViewPort().getWidth(), IRR.driver->getViewPort().getHeight());

    irr::f32 dx = IRR.receiver.MouseX() / (f32)screenSize.Width;
    irr::f32 dy = IRR.receiver.MouseY() / (f32)screenSize.Height;

    ln.start = IRR.camera->getPosition() + (lefttoright * (dx-0.5f)) + (uptodown * (dy-0.5f));

    ln.end = farLeftUp + (lefttoright * dx) + (uptodown * dy);

    return ln;
}

irr::core::vector3df Mouse::IntersectPlane(core::line3df myLine, irr::f32 yCoord)
{
    irr::f32 xSlope = (myLine.end.X - myLine.start.X) / (myLine.end.Y - myLine.start.Y);
    irr::f32 zSlope = (myLine.end.Z - myLine.start.Z) / (myLine.end.Y - myLine.start.Y);

    irr::f32 xCoord = myLine.start.X + (xSlope * (yCoord - myLine.start.Y));
    irr::f32 zCoord = myLine.start.Z + (zSlope * (yCoord - myLine.start.Y));

    return irr::core::vector3df(xCoord, yCoord, zCoord);
}