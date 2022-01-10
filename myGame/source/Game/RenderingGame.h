#pragma once


#include "Game.h"
#include "../Library/Player.h"

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
    //class TriangleDemo;
	//class ModelFromFile;
	class ObjectDiffuseLight;
	class ShadowMappingDemo;
	class ModelFromFile;

    class RenderingGame : public Game
    {
    public:
        RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        ~RenderingGame();

        virtual void Initialize() override;		
        virtual void Update(const GameTime& gameTime) override;
        virtual void Draw(const GameTime& gameTime) override;


	protected:
        virtual void Shutdown() override;

    private:
		static const XMFLOAT4 BackgroundColor;
        FirstPersonCamera * mCamera;
        //TriangleDemo* mDemo;

		//Define member variables for Keyboard and mouse
		LPDIRECTINPUT8 mDirectInput;

		Keyboard* mKeyboard;
		Mouse*    mMouse;
		ModelFromFile* mTakenObject;
		std::vector<ModelFromFile*> mPickableComponents;
	
		int mScore;
		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;

		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		ShadowMappingDemo* mShadowMappingDemo;
		Player* mPlayer;


		ObjectDiffuseLight* mObjectDiffuseLight;

		void Pick(int sx, int sy, ModelFromFile*);
    };
}