#include "GameComponent.h"
#include "GameTime.h"

namespace Library
{
    RTTI_DEFINITIONS(GameComponent)

    GameComponent::GameComponent()
        : mGame(nullptr), mEnabled(true)
    {
    }

    GameComponent::GameComponent(Game& game)
        : mGame(&game), mEnabled(true)
    {
    }

    GameComponent::~GameComponent()
    {
    }

    Game* GameComponent::GetGame()
    {
        return mGame;
    }

    void GameComponent::SetGame(Game& game)
    {
        mGame = &game;
    }

    bool GameComponent::Enabled() const
    {
        return mEnabled;
    }

    void GameComponent::SetEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

    void GameComponent::Initialize()
    {
    }

    void GameComponent::Update(const GameTime& gameTime)
    {
    }

    void GameComponent::SetPosition(const XMFLOAT3& currentPosition) {
        this->currentPosition = currentPosition;
    }
    void GameComponent::SetRotation(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right) {
        this->forwardVector = forward;
        this->upVector = up;
        this->rightVector = right;
    }
}
