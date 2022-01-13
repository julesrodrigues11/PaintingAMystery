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
#include <iostream>

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
		camera = new FirstPersonCamera(*this);
		commonComponents.push_back(camera);
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

		mFpsComponent = new FpsComponent(*this);
		mFpsComponent->Initialize();
		mRenderStateHelper = new RenderStateHelper(*this);

		mSpriteBatch = new SpriteBatch(mDirect3DDeviceContext);
		mSpriteFont = new SpriteFont(mDirect3DDevice, L"Content\\Fonts\\Arial_14_Regular.spritefont");

		InitializeGame();
		InitializeMenu();
		InitializeCredentials();
		Game::Initialize();

		SetState(GameState::Menu);
		

		auto gameStartPosition = XMFLOAT3(0.0f, 13.0f, 0.0f);
		ResetPosition(gameStartPosition);
		nonGamePosition = { gameStartPosition, forwardVector, upVector, rightVector };
		gamePosition = { gameStartPosition, forwardVector, upVector, rightVector };

	}

	void RenderingGame::InitializeGame() {
		keyboard = new Keyboard(*this, mDirectInput);
		commonComponents.push_back(keyboard);
		mServices.AddService(Keyboard::TypeIdClass(), keyboard);

		mouse = new Mouse(*this, mDirectInput);
		commonComponents.push_back(mouse);
		mServices.AddService(Mouse::TypeIdClass(), mouse);

		shadowMapping = new ShadowMappingDemo(*this, *camera);
		gameComponents.push_back(shadowMapping);

		auto mModel1 = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
		mModel1->SetPosition(-1.57f, -0.0f, -0.0f, 0.005f, 0.0f, 12.0f, -4.0f);
		gameComponents.push_back(mModel1);
		pickableComponents.push_back(mModel1);
	}

	void RenderingGame::InitializeMenu() {
		/*
				auto wall = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
				menuComponents.push_back(wall);

				auto startButton = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
				menuComponents.push_back(startButton);

				auto controlsButton = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
				menuComponents.push_back(controlsButton);

				auto exitButton = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
				menuComponents.push_back(exitButton);
		*/

		
	}

	void RenderingGame::InitializeCredentials() {
		auto wallWithCredentials = new ModelFromFile(*this, *camera, "Content\\Models\\bench.3ds", L"A Bench", 20);
		wallWithCredentials->SetPosition(-1.57f, -0.0f, -0.0f, 0.005f, 0.0f, 13.0f, -4.0f);
		credentialsComponents.push_back(wallWithCredentials);
	}

	void RenderingGame::SetState(const GameState& newState) {
		if (gameState == GameState::Game && newState != GameState::Game) {
			gamePosition = { currentPosition, forwardVector, upVector, rightVector };
			ResetPosition(nonGamePosition[0]);
			SetRotation(nonGamePosition[1], nonGamePosition[2], nonGamePosition[3]);
		}
		if (gameState != GameState::Game && newState == GameState::Game) {
			ResetPosition(gamePosition[0]);
			SetRotation(gamePosition[1], gamePosition[2], gamePosition[3]);
		}
		gameState = newState;
		if (gameState == GameState::Game) {
			gamePosition = { currentPosition, forwardVector, upVector, rightVector };
		}
		
		if (gameState == GameState::Menu) {
			currentComponents = &menuComponents;
		}
		else if (gameState == GameState::Game) {
			currentComponents = &gameComponents;
		}
		else if (gameState == GameState::Credentials) {
			currentComponents = &credentialsComponents;
		}
	}

	void RenderingGame::Shutdown()
	{
		for (auto comp : commonComponents)
		{
			DeleteObject(comp);
		}
		for (auto comp : gameComponents)
		{
			DeleteObject(comp);
		}
		for (auto comp : credentialsComponents)
		{
			DeleteObject(comp);
		}
		
		ReleaseObject(mDirectInput);

		DeleteObject(mFpsComponent);
		DeleteObject(mRenderStateHelper);

		//DeleteObject(mObjectDiffuseLight);

		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);


		Game::Shutdown();
	}

	void RenderingGame::Update(const GameTime& gameTime)
	{
		if (keyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			if (gameState == GameState::Credentials || gameState == GameState::Game) {
				SetState(GameState::Menu);
			}
			else {
				Exit();
			}
		}

		if (gameState == GameState::Game) {
			UpdatePosition(gameTime);
			DrawGame(gameTime);
		}
		else if (gameState == GameState::Menu) {
			UpdateMenu(gameTime);
			DrawMenu(gameTime);
		}
		else if (gameState == GameState::Credentials) {
			mFpsComponent->Update(gameTime);
			Game::Update(gameTime);
		}
	}

	void RenderingGame::UpdateMenu(const GameTime& gameTime) {
		long x = Game::screenX, y = Game::screenY;
		mFpsComponent->setMousePosition(x, y);

		long buttonLeft = Game::ScreenWidth() / 3;
		long buttonRight = 2 * buttonLeft;
		long buttonInBetweenSpace = Game::ScreenHeight() / 10;
		long buttonHeight = 2 * buttonInBetweenSpace;

		enum class ButtonNumber { None, StartButton, CredentialsButton, QuitButton };

		ButtonNumber mouseOnButton = ButtonNumber::None;

		if (x >= buttonLeft && x <= buttonRight) {
			y -= buttonInBetweenSpace;
			if (y >= 0 && y <= 2 * buttonInBetweenSpace + 3 * buttonHeight) {
				if (y <= buttonHeight) {
					mouseOnButton = ButtonNumber::StartButton;
				}
				else if (y >= buttonHeight + buttonInBetweenSpace && y <= 2 * buttonHeight + buttonInBetweenSpace) {
					mouseOnButton = ButtonNumber::CredentialsButton;
				}
				else if (y >= 2 * (buttonHeight + buttonInBetweenSpace)) {
					mouseOnButton = ButtonNumber::QuitButton;
				}
			}
		}

		switch (mouseOnButton) {
		case ButtonNumber::StartButton:
			mFpsComponent->setMenuMode(GameState::GameButton); 
			break;
		case ButtonNumber::CredentialsButton:
			mFpsComponent->setMenuMode(GameState::CredentialsButton);
			break;
		case ButtonNumber::QuitButton:
			mFpsComponent->setMenuMode(GameState::ExitButton);
			break;
		case ButtonNumber::None:
			mFpsComponent->setMenuMode(GameState::Menu);//turn off the lights
		}

		if ((mouse != nullptr) && (mouse->IsButtonDown(MouseButtonsLeft)))
		{
			switch (mouseOnButton) {
			case ButtonNumber::StartButton:
				SetState(GameState::Game);
				break;
			case ButtonNumber::CredentialsButton:
				SetState(GameState::Credentials);
				break;
			case ButtonNumber::QuitButton:
				Exit();
				break;
			}
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
			
			shadowMapping->SetPosition(currentPosition);
			camera->SetPosition(currentPosition);
		}
	}

	void RenderingGame::ResetPosition(const XMFLOAT3 & position) {
		currentPosition = position;
		camera->SetPosition(position);
		shadowMapping->SetPosition(position);
		
	}

	void RenderingGame::SetRotation(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right)
	{
		forwardVector = forward;
		upVector = up;
		rightVector = right;
		camera->SetRotation(forward, up, right);
		shadowMapping->SetRotation(forward, up, right);
		
	}

	void RenderingGame::DrawMenu(const GameTime& gameTime)
	{
		mFpsComponent->Update(gameTime);
		Game::Update(gameTime);

	}

    void RenderingGame::DrawGame(const GameTime &gameTime)
    {
		mFpsComponent->Update(gameTime);
		Game::Update(gameTime);

		if (Game::toPick)
		{
			for (auto& component : pickableComponents) {
				if (component->Visible())
					Pick(Game::screenX, Game::screenY, component);
			}
			Game::toPick = false;
		}
		
		for (auto& component : pickableComponents) {
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
				mTakenObject->SetPosition(-M_PI / 2.0 + angle2, M_PI + angle, 0.0f, 0.001f, position.x + direction.x, position.y + direction.y, position.z + direction.z);
			}
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
			else if (model->Taken() == true)
			{
				model->Release();
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