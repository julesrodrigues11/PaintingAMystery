#pragma once

#include "Common.h"

namespace Library
{
    class Game;
    class GameTime;

    class GameComponent : public RTTI
    {
        RTTI_DECLARATIONS(GameComponent, RTTI)

    public:
        GameComponent();
        GameComponent(Game& game);
        virtual ~GameComponent();

        Game* GetGame();
        void SetGame(Game& game);
        bool Enabled() const;
        void SetEnabled(bool enabled);

        virtual void Initialize();
        virtual void Update(const GameTime& gameTime);

        void SetPosition(const XMFLOAT3& currentPosition);
        void SetRotation(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right);

    protected:
        Game* mGame;
        bool mEnabled;
        XMFLOAT3 currentPosition;
        XMFLOAT3 forwardVector;
        XMFLOAT3 rightVector;
        XMFLOAT3 upVector;

    private:
        GameComponent(const GameComponent& rhs);
        GameComponent& operator=(const GameComponent& rhs);
    };
}
