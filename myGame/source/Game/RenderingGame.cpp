#include "RenderingGame.h"
#include "GameException.h"
#include "FirstPersonCamera.h"
//#include "TriangleDemo.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "ModelFromFile.h"
#include "Door.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"
//#include "ObjectDiffuseLight.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"
#include "ShadowMappingBase.h"
#include "ShadowMappingMenu.h"
#include "ShadowMappingCredits.h"
#include "ShadowMappingEnd.h"

//display score
#include <SpriteFont.h>
#include <sstream>
#include <iostream>

# define M_PI           3.14159265358979323846  /* pi */

namespace Rendering
{

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

		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			Keys[i] = false;
		}
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
		InitializeEndGame();
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

		shadowMapping = new ShadowMappingBase(*this, *camera);
		gameComponents.push_back(shadowMapping);

		//Set of Notes
		auto note1 = new ModelFromFile(*this, *camera, "Content\\Models\\LegalPad.fbx", L"Content\\Textures\\KeyNote.jpg", L"a note", false);
		note1->SetOriginAndPosition(-1.57f, 0.0f, 3.14f, 0.5f, 6.0f, 8.0f, 10.5f);
		note1->blocksMovement = true;
		gameComponents.push_back(note1);
		interactiveComponents.push_back(note1);

		auto note2 = new ModelFromFile(*this, *camera, "Content\\Models\\LegalPad.fbx", L"Content\\Textures\\note2.jpg", L"a note", false);
		note2->SetOriginAndPosition(-1.57f, -0.0f, -0.0f, 0.5f, 12.75f, 9.7f, -19.0f);
		note2->blocksMovement = true;
		gameComponents.push_back(note2);
		interactiveComponents.push_back(note2);

		auto note3 = new ModelFromFile(*this, *camera, "Content\\Models\\LegalPad.fbx", L"Content\\Textures\\painting_note.jpg", L"a note", false);
		note3->SetOriginAndPosition(-1.57f, -0.0f, -0.0f, 0.5f, -12.0f, 6.5f, -5.0f);
		note3->blocksMovement = true;
		gameComponents.push_back(note3);
		interactiveComponents.push_back(note3);

		//Set of Key 1 and Door 1
		auto key1 = new ModelFromFile(*this, *camera, "Content\\Models\\key.fbx", L"Content\\Textures\\key_texture.png", L"a Key", false, 0);
		key1->SetOriginAndPosition(-1.57f, -0.0f, -0.0f, 0.05f, 13.0f, 9.0f, -19.0f);
		gameComponents.push_back(key1);
		interactiveComponents.push_back(key1);

		auto door1 = new Door(*this, *camera, "Content\\Models\\door.fbx", L"Content\\Textures\\door_texture.png", 0, false, L"a Door");
		door1->SetPosition(0.0f, 3.14f, 0.0f, 1.1f, 1.25f, 4.25f, 8.8f);
		gameComponents.push_back(door1);
		doors.push_back(door1);

		//Set of Key 2 and Door 2
		auto key2 = new ModelFromFile(*this, *camera, "Content\\Models\\key.fbx", L"Content\\Textures\\key_texture.png", L"a Key", false, 1);
		key2->SetOriginAndPosition(0.0f, 1.57f, 0.0f, 0.1f, -29.25f, 12.0f, 8.25f);
		gameComponents.push_back(key2);
		interactiveComponents.push_back(key2);

		auto door2 = new Door(*this, *camera, "Content\\Models\\door.fbx", L"Content\\Textures\\door_texture.png", 1, false, L"a Door");
		door2->SetPosition(0.0f, -1.57f, 0.0f, 1.1f, -8.8f, 4.25f, -0.5f);
		gameComponents.push_back(door2);
		doors.push_back(door2);
		
		//Painting
		auto painting = new ModelFromFile(*this, *camera, "Content\\Models\\painting.fbx", L"Content\\Textures\\painting_texture.png", L"a Painting", true, 2);
		painting->SetOriginAndPosition(0.0f, 0.0f, 0.0f, 1.0f, 0.01f, 4.25f, -4.5f);
		gameComponents.push_back(painting);
		interactiveComponents.push_back(painting);
		
		//Secret Door
		auto secretDoor = new Door(*this, *camera, "Content\\Models\\door.fbx", L"Content\\Textures\\secret_door_texture.png", 3, true, L"a Door");
		secretDoor->SetPosition(0.0f, 0.0f, 0.0f, 1.4f, -5.25f, 3.75f, -4.5f);
		gameComponents.push_back(secretDoor);
		
		//Numpad
		auto numpad = new Door(*this, *camera, "Content\\Models\\keypad.fbx", L"Content\\Textures\\keypad_texture.png", 2, false, L"a Numpad", *secretDoor);
		numpad->SetPosition(0.0f, 0.0f, 3.14f, 1.0f, -17.5f, 20.0f, -4.5f);
		gameComponents.push_back(numpad);
		doors.push_back(numpad);

		
		doors.push_back(secretDoor);
		
	}

	void RenderingGame::InitializeMenu() {
		auto shadowMapping = new ShadowMappingMenu(*this, *camera);
		menuComponents.push_back(shadowMapping);
	}

	void RenderingGame::InitializeCredentials() {
		auto shadowMapping = new ShadowMappingCredits(*this, *camera);
		credentialsComponents.push_back(shadowMapping);
	}

	void RenderingGame::InitializeEndGame() {
		auto shadowMapping = new ShadowMappingEnd(*this, *camera);
		endComponents.push_back(shadowMapping);
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
		else if (gameState == GameState::Credentials || gameState == GameState::FinalCredits) {
			currentComponents = &credentialsComponents;
		}
		else if (gameState == GameState::EndGame) {
			currentComponents = &endComponents;
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
			else if (gameState == GameState::FinalCredits) {
				SetState(GameState::EndGame);
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
		else if (gameState == GameState::Credentials || gameState == GameState::FinalCredits) {
			mFpsComponent->Update(gameTime);
			Game::Update(gameTime);
		} else {
			mFpsComponent->Update(gameTime);
			UpdateEnd(gameTime);
			Game::Update(gameTime);
		}
	}

	void RenderingGame::UpdateMenu(const GameTime& gameTime) {
		long x = Game::screenX, y = Game::screenY;
		mFpsComponent->setMousePosition(x, y);

		long buttonLeft = Game::ScreenWidth() / 2.75;
		long buttonRight = 1.72 * buttonLeft;
		long upperGap = Game::ScreenHeight() / 2.25;
		long buttonInBetweenSpace = Game::ScreenHeight() / 25;
		long buttonHeight = 2.5 * Game::ScreenHeight() / 20;

		enum class ButtonNumber { None, StartButton, CredentialsButton, QuitButton };

		ButtonNumber mouseOnButton = ButtonNumber::None;

		if (x >= buttonLeft && x <= buttonRight) {
			y -= upperGap;
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

	void RenderingGame::UpdateEnd(const GameTime& gameTime) {
		long x = Game::screenX, y = Game::screenY;
		mFpsComponent->setMousePosition(x, y);

		long buttonLeft = Game::ScreenWidth() / 2.5;
		long buttonRight = 1.5 * buttonLeft;
		long buttonInBetweenSpace = Game::ScreenHeight() / 25;
		long upperGap = 2 * Game::ScreenHeight() / 3 - buttonInBetweenSpace / 2;
		long buttonHeight = 2.5 * Game::ScreenHeight() / 20 - buttonInBetweenSpace / 2;

		enum class ButtonNumber { None, StartButton, CredentialsButton, QuitButton };

		ButtonNumber mouseOnButton = ButtonNumber::None;

		if (x >= buttonLeft && x <= buttonRight) {
			y -= upperGap;
			if (y >= 0 && y <= 2 * buttonInBetweenSpace + 3 * buttonHeight) {
				if (y <= buttonHeight) {
					mouseOnButton = ButtonNumber::CredentialsButton;
				}
				else if (y >= buttonHeight + buttonInBetweenSpace && y <= 2 * buttonHeight + buttonInBetweenSpace) {
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
			case ButtonNumber::CredentialsButton:
				SetState(GameState::FinalCredits);
				break;
			case ButtonNumber::QuitButton:
				Exit();
				break;
			}
		}

	}

	void RenderingGame::DropObject() {
		if (mTakenObject != nullptr && mTakenObject->KeyID() == -1)
		{
			mTakenObject->ResetToOrigin();
			mTakenObject = nullptr;
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

			if (mouse->IsButtonDown(MouseButtonsLeft))
			{
				DropObject();
			}
		}

		XMFLOAT2 rotationAmount = XMFLOAT2(0.0f, 0.0f);


		if ((mouse != nullptr) && (mouse->IsButtonHeldDown(MouseButtonsLeft)))
		{
			LPDIMOUSESTATE mouseState = mouse->CurrentState();
			rotationAmount.x = -mouseState->lX * 100.0f;
			rotationAmount.y = -mouseState->lY * 100.0f;
		}
		if (mTakenObject == nullptr || !mTakenObject->blocksMovement) {
			float elapsedTime = (float)gameTime.ElapsedGameTime();
			ApplyRotation(elapsedTime, rotationAmount);
			Move(elapsedTime, movementAmount);
		}
		
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

		if (Game::toOpen)
		{
			for (auto& component : doors)
			{
				if (component->getLocked())
				{
					OpenDoor(Game::screenX, Game::screenY, component);
				}
			}
			Game::toOpen = false;
		}

		if (Game::toPick)
		{
			for (auto& component : interactiveComponents) {
				if (component->Visible())
					Interact(Game::screenX, Game::screenY, component);
			}
			Game::toPick = false;
		}
		
		for (auto& component : interactiveComponents) {
			if (component->Taken()) {
				mTakenObject = component;
				component->Release();
				if (component->KeyID() != -1) {
					currentComponents->erase(std::remove((*currentComponents).begin(), (*currentComponents).end(), component), (*currentComponents).end());
				}
			}
		}

		if (mTakenObject != nullptr && mTakenObject->KeyID() == -1) {
			const auto& position = camera->Position();
			const auto& direction = camera->Direction();
			if (direction.x == 0 && direction.z == 0) {
				mTakenObject->SetPosition(0.0f, -direction.y, 0.0f, 0.001f, position.x + direction.x, position.y + direction.y, position.z + direction.z);
			}
			else {
				auto angle = atan2(direction.x, direction.z);
				auto hype = sqrt(direction.x * direction.x + direction.z * direction.z);
				auto angle2 = atan(direction.y / hype);
				mTakenObject->SetPosition(-M_PI / 2.0 + angle2 + M_PI / 3.0, M_PI + angle , 0.0f, 0.5f, position.x + direction.x, position.y + direction.y, position.z + direction.z);
			}
		}
		
		
	}

	void RenderingGame::Interact(int sx, int sy, ModelFromFile* model)
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

		if (model->isPainting() == false)
		{
			if (model->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream pickupString;
				pickupString << L"You have picked up " << (model->GetModelDes()).c_str() << '!';

				int result = MessageBox(0, pickupString.str().c_str(), L"Object picked up", MB_ICONASTERISK);
				model->Take();

				if (model->KeyID() != -1)
				{
					Keys[model->KeyID()] = true;
				}
			}
		}
		else
		{
			if (model->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				if (Keys[1] == false)
				{
					std::wostringstream pickupString;
					pickupString << L"This is a replica of Starry Nights by Van Gogh";

					int result = MessageBox(0, pickupString.str().c_str(), L"Painting observed", MB_ICONASTERISK | MB_OK);
				}
				else
				{
					std::wostringstream pickupString;
					pickupString << L"Upon closer inspection, there seem to be numbers written over this painting";

					int result = MessageBox(0, pickupString.str().c_str(), L"Painting observed", MB_ICONASTERISK | MB_OK);

					Keys[model->KeyID()] = true;
				}
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

		
		mRenderStateHelper->RestoreAll();

       
        HRESULT hr = mSwapChain->Present(0, 0);
        if (FAILED(hr))
        {
            throw GameException("IDXGISwapChain::Present() failed.", hr);
        }


    }

	//Function to Open Door
	void RenderingGame::OpenDoor(int sx, int sy, Door* door)
	{
		//XMMATRIX P = mCam.Proj(); 
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, camera->ProjectionMatrix());


		//Compute picking ray in view space.
		float vx = (+2.0f * sx / Game::DefaultScreenWidth - 1.0f) / P(0, 0);
		float vy = (-2.0f * sy / Game::DefaultScreenHeight + 1.0f) / P(1, 1);

		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, -1.0f, 0.0f);

		// Tranform ray to local space of Mesh via the inverse of both of view and world transform

		XMMATRIX V = camera->ViewMatrix();
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);


		XMMATRIX W = XMLoadFloat4x4(door->WorldMatrix());
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		// Make the ray direction unit length for the intersection tests.
		rayDir = XMVector3Normalize(rayDir);


		float tmin = 0.0;

		if (door->isSecret())
		{
			if (door->getLocked() && Keys[door->getID()] == false && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream lockedString;
				lockedString << L"This wall feels like it moves.\nThere must be some way to open it?" << '\n' << '\t';

				int result = MessageBox(0, lockedString.str().c_str(), L"Mysterious Wall", MB_ICONASTERISK | MB_OK);

			}
			else if (door->getLocked() && Keys[door->getID()] && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream unlockedString;
				unlockedString << L"Would you like to slide this open?" << '\n' << '\t';

				int result = MessageBox(0, unlockedString.str().c_str(), L"Slide Wall?", MB_ICONASTERISK | MB_YESNO);

				if (result == IDYES)
				{
					door->setLocked(false);
					SetState(GameState::EndGame);
				}
			}
		}
		else if (!door->isNumpad())
		{
			if (door->getLocked() && Keys[door->getID()] == false && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream lockedString;
				lockedString << L"This door is locked. Find a key to unlock this door." << '\n' << '\t';

				int result = MessageBox(0, lockedString.str().c_str(), L"Locked Door", MB_ICONASTERISK | MB_OK);

			}
			else if (door->getLocked() && Keys[door->getID()] && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream unlockedString;
				unlockedString << L"Would you like to open this door?" << '\n' << '\t';

				int result = MessageBox(0, unlockedString.str().c_str(), L"Open Door?", MB_ICONASTERISK | MB_YESNO);

				if (result == IDYES)
				{
					door->setLocked(false);
					setGameOver(true);
				}
			}
		}
		else
		{
			if (door->getLocked() && Keys[door->getID()] == false && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream lockedString;
				lockedString << L"This numpad requires a code. But you don't have the code." << '\n' << '\t';

				int result = MessageBox(0, lockedString.str().c_str(), L"Locked Door", MB_ICONASTERISK | MB_OK);

			}
			else if (door->getLocked() && Keys[door->getID()] && door->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
			{
				std::wostringstream unlockedString;
				unlockedString << L"Would you like to enter the code from the painting?" << '\n' << '\t';

				int result = MessageBox(0, unlockedString.str().c_str(), L"Open Door?", MB_ICONASTERISK | MB_YESNO);

				if (result == IDYES)
				{
					Keys[ARRAY_SIZE - 1] = true;
				}
			}
		}

	}

	void RenderingGame::setGameOver(bool GameOver)
	{
		this->gameOver = GameOver;
	}
}