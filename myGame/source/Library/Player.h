#pragma once
#include "../Game/ShadowMappingBase.h"
#include "GameComponent.h"
#include "GameTime.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "FirstPersonCamera.h"

namespace Library
{
    class Keyboard;
    class Mouse;
    class PointLight;
    class Camera;
    class Player : public GameComponent
    {
        RTTI_DECLARATIONS(Player, GameComponent)

    public:
        Player(Game& game);
        Player(Game& game, Keyboard& keyboard, Mouse& mouse, Camera& camera, Rendering::ShadowMappingBase& shadowMapping);

        virtual ~Player();

        const Keyboard& GetKeyboard() const;
        void SetKeyboard(Keyboard& keyboard);

        const Mouse& GetMouse() const;
        void SetMouse(Mouse& mouse);

        const Rendering::ShadowMappingBase& GetShadowMapping() const;
        void SetShadowMapping(Rendering::ShadowMappingBase& mouse);


        virtual void Initialize() override;
        virtual void Update(const GameTime& gameTime) override;

        void ApplyRotation(float elapsedTime, XMFLOAT2 rotation);
        void Move(float elapsedTime, XMFLOAT3 movementAmount);

        void ResetPosition(const XMFLOAT3 &position);

    protected:

        XMFLOAT3 currentPosition;
        XMFLOAT3 forwardVector;
        XMFLOAT3 rightVector;
        XMFLOAT3 upVector;

        Keyboard* keyboard;
        Mouse* mouse;
        Camera* camera;
        Rendering::ShadowMappingBase* shadowMapping;

    private:
        Player(const Player& rhs);
        Player& operator=(const Player& rhs);
    };
}



