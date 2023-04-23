#pragma once


#include "Game.h"
#include "../Library/Player.h"
#include "../Library/ModelFromFile.h"
#include "../Library/FpsComponent.h"
#include "../Library/Door.h"

using namespace Library;

namespace Library
{
    class FirstPersonCamera;
	class RenderStateHelper;

	class Keyboard;
	class Mouse;
	class FpsComponent;

}

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}


namespace Rendering
{

	const float DefaultRotationRate = XMConvertToRadians(1.0f);
	const float DefaultMovementRate = 10.0f;
	const float DefaultMouseSensitivity = 100.0f;
	const XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	const int ARRAY_SIZE = 4;

    //class TriangleDemo;
	//class ModelFromFile;
	class ObjectDiffuseLight;
	class ShadowMappingBase;

    class RenderingGame : public Game
    {
    public:
        RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        ~RenderingGame();

        virtual void Initialize() override;
		void InitializeGame();
		void InitializeMenu();
		void InitializeCredentials();
		void InitializeEndGame();
		void SetState(const GameState& newState);
        virtual void Update(const GameTime& gameTime) override;
		void UpdateMenu(const GameTime& gameTime);
		void UpdateEnd(const GameTime& gameTime);
		void DropObject();
		void UpdatePosition(const GameTime& gameTime);
		void ApplyRotation(float elapsedTime, XMFLOAT2 rotation);
		void Move(float elapsedTime, XMFLOAT3 movementAmount);
		void ResetPosition(const XMFLOAT3& position);
		void SetRotation(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right);
		void DrawMenu(const GameTime& gameTime);
		void DrawGame(const GameTime& gameTime);
        virtual void Draw(const GameTime& gameTime) override;
		GameState gameState;

		int Keys[ARRAY_SIZE];

		void setGameOver(bool GameOver);


	protected:
        virtual void Shutdown() override;
		std::vector<XMFLOAT3> gamePosition;
		std::vector<XMFLOAT3> nonGamePosition;

		XMFLOAT3 currentPosition;
		XMFLOAT3 forwardVector;
		XMFLOAT3 rightVector;
		XMFLOAT3 upVector;
		XMFLOAT2 mousePosition;


    private:
		
		static const XMFLOAT4 BackgroundColor;
        FirstPersonCamera * camera;
        //TriangleDemo* mDemo;

		//Define member variables for Keyboard and mouse
		LPDIRECTINPUT8 mDirectInput;

		Keyboard* keyboard;
		Mouse*    mouse;
		ModelFromFile* mTakenObject;
		std::vector<ModelFromFile*> interactiveComponents;
		std::vector<Door*> doors;
	
		int mScore;
		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;

		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		ShadowMappingBase* shadowMapping;
		Player* mPlayer;


		ObjectDiffuseLight* mObjectDiffuseLight;

		void Interact(int sx, int sy, ModelFromFile*);
		void OpenDoor(int sx, int sy, Door* door);

		bool gameOver = false;
    };
}