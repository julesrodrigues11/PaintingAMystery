#pragma once

#include "DrawableGameComponent.h"

namespace DirectX
{
    class SpriteBatch;
    class SpriteFont;
}

namespace Library
{

    enum class GameState { Menu, Game, Credentials, ExitButton, GameButton, CredentialsButton, FinalCredits, EndGame};
    class FpsComponent : public DrawableGameComponent
    {
        RTTI_DECLARATIONS(FpsComponent, DrawableGameComponent)

    public:
        FpsComponent(Game& game);
        ~FpsComponent();

        XMFLOAT2& TextPosition();
        int FrameRate() const;

        virtual void Initialize() override;
        virtual void Update(const GameTime& gameTime) override;
        virtual void Draw(const GameTime& gameTime) override;
        void setMenuMode(GameState state);
        void setMousePosition(float x, float y);

    private:
        FpsComponent();
        FpsComponent(const FpsComponent& rhs);
        FpsComponent& operator=(const FpsComponent& rhs);
        
        SpriteBatch* mSpriteBatch;
        SpriteFont* mSpriteFont;
        XMFLOAT2 mTextPosition;
        XMFLOAT2 mousePosition;
        GameState gameState = GameState::Menu;

        int mFrameCount;
        int mFrameRate;
        double mLastTotalElapsedTime;
    };
}