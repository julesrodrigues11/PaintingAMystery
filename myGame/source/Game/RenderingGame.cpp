#include "RenderingGame.h"
#include "GameException.h"
#include "FirstPersonCamera.h"
//#include "TriangleDemo.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "ModelFromFile.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"
//#include "ObjectDiffuseLight.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"
#include "ShadowMappingDemo.h"

//display score
#include <SpriteFont.h>
#include <sstream>

# define M_PI           3.14159265358979323846  /* pi */

namespace Rendering
{;

	const XMFLOAT4 RenderingGame::BackgroundColor = { 0.75f, 0.75f, 0.75f, 1.0f };


	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
		: Game(instance, windowClass, windowTitle, showCommand),
		mDirectInput(nullptr), keyboard(nullptr), mouse(nullptr),
		mFpsComponent(nullptr), mRenderStateHelper(nullptr), shadowMapping(nullptr)
		/*mDemo(nullptr), mDirectInput(nullptr), mKeyboard(nullptr), mMouse(nullptr), mModel1(nullptr), mModel2(nullptr),
		mFpsComponent(nullptr), mRenderStateHelper(nullptr), mObjectDiffuseLight(nullptr)*/
    {
        mDepthStencilBufferEnabled = true;
        mMultiSamplingEnabled = true;
    }

    RenderingGame::~RenderingGame()
    {
    }

    void RenderingGame::Initialize()
    {	
		gameState = GameState::Game;
        camera = new FirstPersonCamera(*this);
        mComponents.push_back(camera);
        mServices.AddService(Camera::TypeIdClass(), camera);

		currentPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		rightVector = XMFLOAT3(1.0f, 0.0f, 0.0f);
		forwardVector = XMFLOAT3(0.0f, 0.0f, -1.0f);
		upVector = Up;
    
        //mDemo = new TriangleDemo(*this, *mCamera);
       // mComponents.push_back(mDemo);

		//Remember that the component is a management class for all objects in the D3D rendering engine. 
		//It provides a centralized place to create and release objects. 
	    //NB: In C++ and other similar languages, to instantiate a class is to create an object.
		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput, nullptr)))
		{
			throw GameException("DirectInput8Create() failed");
		}

		keyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(keyboard);
		mServices.AddService(Keyboard::TypeIdClass(), keyboard);

		mouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mouse);
		mServices.AddService(Mouse::TypeIdClass(), mouse);

		
		
		auto mModel1 = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench",20);
		mModel1->SetPosition(-1.57f, -0.0f, -0.0f, 0.005f, 0.0f, 12.0f, 0.0f);
		mComponents.push_back(mModel1);
		mPickableComponents.push_back(mModel1);

		//mModel2 = new ModelFromFile(*this, *mCamera, "Content\\Models\\bench.3ds", L"A tree", 10);
		//mModel2->SetPosition(-1.57f, -0.0f, -0.0f, 0.005f, 5.0f, 0.4f, 0.0f);
		//mComponents.push_back(mModel2);


		//house object with diffuse lighting effect:
		//mObjectDiffuseLight = new ObjectDiffuseLight(*this, *mCamera, "Content\\Models\\HepBurn_Sofa.3ds");
		//mObjectDiffuseLight->SetPosition(-1.57f, -0.0f, -0.0f, 0.01f, -1.0f, 0.75f, -2.5f);
		//mComponents.push_back(mObjectDiffuseLight);
		//RasterizerStates::Initialize(mDirect3DDevice);
		//SamplerStates::Initialize(mDirect3DDevice);

		shadowMapping = new ShadowMappingDemo(*this, *camera);
		//mShadowMappingDemo->Initialise();
		mComponents.push_back(shadowMapping);

		//mPlayer = new Player(*this, *keyboard, *mouse, *camera, *shadowMapping);
		//mComponents.push_back(mPlayer);
	
		mFpsComponent = new FpsComponent(*this);
		mFpsComponent->Initialize();
		mRenderStateHelper = new RenderStateHelper(*this);
		
		
		mSpriteBatch = new SpriteBatch(mDirect3DDeviceContext);
		mSpriteFont = new SpriteFont(mDirect3DDevice, L"Content\\Fonts\\Arial_14_Regular.spritefont");


		Game::Initialize();

		auto gameStartPosition = XMFLOAT3(0.0f, 13.0f, 0.0f);
		ResetPosition(gameStartPosition);
        //mCamera->SetPosition(0.0f, 13.0f, 0.0f);
    }

    void RenderingGame::Shutdown()
    {
		

		
		//DeleteObject(mDemo);
        DeleteObject(camera);
		
		
		DeleteObject(keyboard);
		DeleteObject(mouse);
		//DeleteObject(mPlayer);
		ReleaseObject(mDirectInput);
		
		for (auto comp : mPickableComponents) 
		{
			DeleteObject(comp);
		}
		//DeleteObject(mModel2);
	

		DeleteObject(mFpsComponent);
		DeleteObject(mRenderStateHelper);

		//DeleteObject(mObjectDiffuseLight);

		DeleteObject(shadowMapping);
		
		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);


        Game::Shutdown();
    }

	void RenderingGame::Update(const GameTime& gameTime)
	{
		if (gameState == GameState::Game) {
			UpdatePosition(gameTime);
			DrawGame(gameTime);
		}
		else if (gameState == GameState::Menu) {
			DrawMenu(gameTime);
		}
	}

	void RenderingGame::UpdatePosition(const GameTime& gameTime) {
		//const XMFLOAT3 Vector3Helper::Forward = XMFLOAT3(0.0f, 0.0f, -1.0f);
		XMFLOAT3 movementAmount = XMFLOAT3(0.0f, 0.0f, 0.0f);

		if (keyboard != nullptr)
		{
			if (keyboard->IsKeyDown(DIK_W))
			{
				movementAmount.z = 1.0f;
			}

			if (keyboard->IsKeyDown(DIK_S))
			{
				movementAmount.z = -1.0f;
			}

			if (keyboard->IsKeyDown(DIK_A))
			{
				movementAmount.x = -1.0f;
			}

			if (keyboard->IsKeyDown(DIK_D))
			{
				movementAmount.x = 1.0f;
			}
		}

		XMFLOAT2 rotationAmount = XMFLOAT2(0.0f, 0.0f);


		if ((mouse != nullptr) && (mouse->IsButtonHeldDown(MouseButtonsLeft)))
		{
			LPDIMOUSESTATE mouseState = mouse->CurrentState();
			rotationAmount.x = -mouseState->lX * 100.0f;
			rotationAmount.y = -mouseState->lY * 100.0f;
		}
		float elapsedTime = (float)gameTime.ElapsedGameTime();
		ApplyRotation(elapsedTime, rotationAmount);
		Move(elapsedTime, movementAmount);
	}

	void RenderingGame::ApplyRotation(float elapsedTime, XMFLOAT2 rotation) {
		if (rotation.x != 0 || rotation.y != 0) {
			XMVECTOR rotationVector = XMLoadFloat2(&rotation) * DefaultRotationRate * elapsedTime;
			XMVECTOR forward = XMLoadFloat3(&forwardVector);
			XMVECTOR right = XMLoadFloat3(&rightVector);
			XMVECTOR up = XMLoadFloat3(&upVector);

			XMMATRIX pitchMatrix = XMMatrixRotationAxis(right, XMVectorGetY(rotationVector));
			XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));


			auto rotationMatrix = XMMatrixMultiply(pitchMatrix, yawMatrix);

			up = XMVector3TransformNormal(up, rotationMatrix);
			up = XMVector3Normalize(up);

			forward = XMVector3TransformNormal(forward, rotationMatrix);
			forward = XMVector3Normalize(forward);

			right = XMVector3Cross(forward, up);

			//XMVector3TransformNormal(right, rotationMatrix);
			// right = XMVector3Normalize(right);

			XMStoreFloat3(&forwardVector, forward);
			XMStoreFloat3(&upVector, up);
			XMStoreFloat3(&rightVector, right);
			shadowMapping->SetRotation(forwardVector, upVector, rightVector);
			camera->SetRotation(forwardVector, upVector, rightVector);
		}
	}

	void RenderingGame::Move(float elapsedTime, XMFLOAT3 movementAmount) {
		if (movementAmount.x != 0 || movementAmount.y != 0 || movementAmount.z != 0) {
			XMVECTOR position = XMLoadFloat3(&currentPosition);
			XMVECTOR movement = XMLoadFloat3(&movementAmount) * DefaultMovementRate * elapsedTime;

			auto mStraightDirection = forwardVector;
			mStraightDirection.y = 0;

			XMStoreFloat3(&mStraightDirection, XMVector3Normalize(XMLoadFloat3(&mStraightDirection)));
			XMVECTOR fixedRight;

			fixedRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&mStraightDirection), XMLoadFloat3(&Up)));


			XMVECTOR strafe = fixedRight * XMVectorGetX(movement);
			position += strafe;

			XMVECTOR forward = XMLoadFloat3(&mStraightDirection) * XMVectorGetZ(movement);
			position += forward;

			XMStoreFloat3(&currentPosition, position);
			camera->SetPosition(currentPosition);
			shadowMapping->SetPosition(currentPosition);
		}
	}

	void RenderingGame::ResetPosition(const XMFLOAT3 & position) {
		currentPosition = position;
		camera->SetPosition(position);
		shadowMapping->SetPosition(currentPosition);
	}

	void RenderingGame::DrawMenu(const GameTime& gameTime)
	{
	}

    void RenderingGame::DrawGame(const GameTime &gameTime)
    {

		mFpsComponent->Update(gameTime);
		Game::Update(gameTime);
		//Add "ESC" to exit the application
		if (keyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			Exit();
		}


		//bounding box , we need to see if we need to do the picking test
		for (auto& component : mPickableComponents) {
			if (component->Taken()) {
				mTakenObject = component;
				component->Release();
			}
		}

		if (mTakenObject != nullptr) {
			const auto& position = camera->Position();
			const auto& direction = camera->Direction();
			if (direction.x == 0 && direction.z == 0) {
				mTakenObject->SetPosition(0.0f, -direction.y, 0.0f, 0.001f, position.x + direction.x, position.y + direction.y, position.z + direction.z);
			}
			else {
				auto angle = atan2(direction.x, direction.z);
				auto hype = sqrt(direction.x * direction.x + direction.z * direction.z);
				auto angle2 = atan(direction.y / hype);
				mTakenObject->SetPosition(-M_PI/2.0 + angle2, M_PI+angle, 0.0f, 0.001f, position.x + direction.x, position.y + direction.y, position.z + direction.z);


			}
		}
		
		if (Game::toPick)
		{
			for (auto& component : mPickableComponents) {
				if (component->Visible())
					Pick(Game::screenX, Game::screenY, component);
			}
			Game::toPick = false;
		}

	}


	// do the picking here

	void RenderingGame::Pick(int sx, int sy, ModelFromFile* model)
	{
		//XMMATRIX P = mCam.Proj(); 
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, camera->ProjectionMatrix());


		//Compute picking ray in view space.
		float vx = (+2.0f*sx / Game::DefaultScreenWidth - 1.0f) / P(0, 0);
		float vy = (-2.0f*sy / Game::DefaultScreenHeight + 1.0f) / P(1, 1);

		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, -1.0f, 0.0f);

		// Tranform ray to local space of Mesh via the inverse of both of view and world transform
		
		XMMATRIX V = camera->ViewMatrix();
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

		
		XMMATRIX W = XMLoadFloat4x4(model->WorldMatrix());
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		// Make the ray direction unit length for the intersection tests.
		rayDir =  XMVector3Normalize(rayDir);

	
	
		float tmin = 0.0;
		if (model->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
		{
			std::wostringstream pickupString;
			pickupString << L"Do you want to pick up: " << (model->GetModelDes()).c_str()<<'\n'<<'\t'<<'+'<<model->ModelValue()<<L" points";
			
			int result = MessageBox(0, pickupString.str().c_str(), L"Object Found", MB_ICONASTERISK | MB_YESNO);

			if (result == IDYES)
			{ 
				model->Take();
			}
		
		}
	}
	

	

    void RenderingGame::Draw(const GameTime &gameTime)
    {
        mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
        mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        Game::Draw(gameTime);
		mRenderStateHelper->SaveAll();
		mFpsComponent->Draw(gameTime);
		
		mSpriteBatch->Begin();
		//draw the score
		std::wostringstream scoreLabel;
		scoreLabel << L"Your current score: " << mScore << "\n";
		mSpriteFont->DrawString(mSpriteBatch, scoreLabel.str().c_str(), XMFLOAT2(0.0f, 120.0f), Colors::Red);
		mSpriteBatch->End();

		
		mRenderStateHelper->RestoreAll();

       
        HRESULT hr = mSwapChain->Present(0, 0);
        if (FAILED(hr))
        {
            throw GameException("IDXGISwapChain::Present() failed.", hr);
        }


		

    }
}