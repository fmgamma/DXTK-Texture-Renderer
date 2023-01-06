#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


//one time setup, configure a box and a quad
void Game::Initialise()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	//wood floor
	mQuad.Initialise(BuildQuad(d3d.GetMeshMgr()));
	
	//change the defatult material inside the mesh
	Material& matQ = mQuad.GetMesh().GetSubMesh(0).material;
	matQ.gfxData.Set(Vector4(1.f, 1.f, 1.f, 1), Vector4(1.f, 1.f, 1.f, 1), Vector4(0.9f, 0.8f, 0.8f, 1));
	matQ.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "floor.dds");
	matQ.texture = "floor.dds";

	//pandoras box
	mBox.Initialise(BuildCube(d3d.GetMeshMgr()));
	//change the default material inside the mesh
	Material& matB = mBox.GetMesh().GetSubMesh(0).material;
	matB.gfxData.Set(Vector4(1.0f, 0.01f, 0.01f, 1), Vector4(0.9f, 0.1f, 0.1f, 1), Vector4(0.9f, 0.1f, 0.1f, 1));

	//the sun						 LightLDX, bl_enable, Direction, Diffusion, Ambient, Specular
	d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));
}

//tidy up
void Game::Release()
{
}

void Game::Update(float dTime)
{
	//spin the box
	gAngle += dTime * 0.5f;
	mBox.GetRotation().y = gAngle;
}

void Game::Render(float dTime)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.BeginRender(Colours::Blue);

	float alpha = 0.5f + sinf(gAngle * 2)*0.5f;
	//setup the shaders, camera and projection
	d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
	CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f*PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);

	//main cube 
	d3d.GetFX().Render(mBox);

	//floor
	mQuad.GetRotation() = Vector3(0, 0, 0);
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, -1, 0);
	d3d.GetFX().Render(mQuad);


	//walls
	/*mQuad.GetRotation() = Vector3(0, 0, 0);
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, 1, 0);
	d3d.GetFX().Render(mQuad);

	mQuad.GetRotation() = Vector3(0, 0, 0);
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, 2, 0);
	d3d.GetFX().Render(mQuad);*/
	

	d3d.EndRender();
}

//push the camera and check for exit
LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 200.f * GetElapsedSec();
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		case 'a':
			mCamPos.y += camInc;
			break;
		case 'z':
			mCamPos.y -= camInc;
			break;
		case 'd':
			mCamPos.x -= camInc;
			break;
		case 'f':
			mCamPos.x += camInc;
			break;
		case 'w':
			mCamPos.z += camInc;
			break;
		case 's':
			mCamPos.z -= camInc;
			break;
		case ' ':
			mCamPos = mDefCamPos;
			break;
		}
	}
	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

