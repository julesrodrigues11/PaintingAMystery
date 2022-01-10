#pragma once


#include "Game.h"
#include "../Library/Player.h"
#include "../Library/ModelFromFile.h"

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

	enum class GameState {Menu, Game};
    //class TriangleDemo;
	//class ModelFromFile;
	class ObjectDiffuseLight;
	class ShadowMappingDemo;

    class RenderingGame : public Game
    {
    public:
        RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        ~RenderingGame();

        virtual void Initialize() override;		
        virtual void Update(const GameTime& gameTime) override;
		void UpdatePosition(const GameTime& gameTime);
		void ApplyRotation(float elapsedTime, XMFLOAT2 rotation);
		void Move(float elapsedTime, XMFLOAT3 movementAmount);
		void ResetPosition(const XMFLOAT3& position);
		void DrawMenu(const GameTime& gameTime);
		void DrawGame(const GameTime& gameTime);
        virtual void Draw(const GameTime& gameTime) override;
		GameState gameState;


	protected:
        virtual void Shutdown() override;
		XMFLOAT3 currentPosition;
		XMFLOAT3 forwardVector;
		XMFLOAT3 rightVector;
		XMFLOAT3 upVector;


    private:
		
		static const XMFLOAT4 BackgroundColor;
        FirstPersonCamera * camera;
        //TriangleDemo* mDemo;

		//Define member variables for Keyboard and mouse
		LPDIRECTINPUT8 mDirectInput;

		Keyboard* keyboard;
		Mouse*    mouse;
		ModelFromFile* mTakenObject;
		std::vector<ModelFromFile*> mPickableComponents;
	
		int mScore;
		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;

		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		ShadowMappingDemo* shadowMapping;
		Player* mPlayer;


		ObjectDiffuseLight* mObjectDiffuseLight;

		void Pick(int sx, int sy, ModelFromFile*);
    };
}