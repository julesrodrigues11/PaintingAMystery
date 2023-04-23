#include "FpsComponent.h"
#include <sstream>
#include <iomanip>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "Game.h"
#include "Utility.h"
#include <string>

namespace Library
{
    RTTI_DEFINITIONS(FpsComponent)

    FpsComponent::FpsComponent(Game& game)
        : DrawableGameComponent(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 20.0f),
          mFrameCount(0), mFrameRate(0), mLastTotalElapsedTime(0.0)
    {
    }
    
    FpsComponent::~FpsComponent()
    {
        DeleteObject(mSpriteFont);
        DeleteObject(mSpriteBatch);
    }

    XMFLOAT2& FpsComponent::TextPosition()
    {
        return mTextPosition;
    }

    int FpsComponent::FrameRate() const
    {
        return mFrameCount;
    }

    void FpsComponent::Initialize()
    {       
        SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

        mSpriteBatch = new SpriteBatch(mGame->Direct3DDeviceContext());
        mSpriteFont = new SpriteFont(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");
    }

    void FpsComponent::Update(const GameTime& gameTime)
    {
        if (gameTime.TotalGameTime() - mLastTotalElapsedTime >= 1)
        {
            mLastTotalElapsedTime = gameTime.TotalGameTime();
            mFrameRate = mFrameCount;
            mFrameCount = 0;
        }

        mFrameCount++;
    }

   

    void FpsComponent::Draw(const GameTime& gameTime)
    {
        
        mSpriteBatch->Begin();
        std::wostringstream fpsLabel;
        //fpsLabel << std::setprecision(4) << L"Frame Rate: " << mFrameRate <<"Total Elapsed Time: " << gameTime.TotalGameTime();
        fpsLabel << std::setprecision(4) << L"Mouse coordinates: " << mousePosition.x <<"; " << std::setprecision(4) << mousePosition.y;
        auto stringState = "";
        switch (gameState) {
        case GameState::Menu:
            stringState = "Menu";
            break;
        case GameState::Game:
            stringState = "Game";
            break;
        case GameState::GameButton:
            stringState = "GameButton";
            break;
        case GameState::ExitButton:
            stringState = "ExitButton";
            break;
        case GameState::Credentials:
            stringState = "Credentials";
            break;
        case GameState::CredentialsButton:
            stringState = "CredentialsButton";
            break;

        }
        fpsLabel << "Game state is: " << stringState;
       // mSpriteFont->DrawString(mSpriteBatch, fpsLabel.str().c_str(), mTextPosition);

        mSpriteBatch->End();
    }

    void FpsComponent::setMenuMode(GameState state) {
        gameState = state;
    }

    void FpsComponent::setMousePosition(float x, float y) {
        mousePosition = XMFLOAT2(x, y);
    }
}