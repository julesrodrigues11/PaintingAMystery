#include "Player.h"
#include "VectorHelper.h"

namespace Library
{
    RTTI_DEFINITIONS(Player)

    const float DefaultRotationRate = XMConvertToRadians(1.0f);
    const float DefaultMovementRate = 10.0f;
    const float DefaultMouseSensitivity = 100.0f;

    Player::~Player()
    {
    }

    Player::Player(Game& game)
        : GameComponent(game), keyboard(nullptr), mouse(nullptr), shadowMapping(nullptr), camera(nullptr)
    {
    }


    Player::Player(Game& game, Keyboard& keyboard, Mouse& mouse, Camera& camera, Rendering::ShadowMappingDemo& shadowMapping)
        : GameComponent(game), keyboard(&keyboard), mouse(&mouse), camera(&camera), shadowMapping(&shadowMapping)
    {
    }

    const Keyboard& Player::GetKeyboard() const
    {
        return *keyboard;
    }

    void Player::SetKeyboard(Keyboard& keyboard)
    {
        this->keyboard = &keyboard;
    }


    void Player::SetMouse(Mouse& mouse)
    {
        this->mouse = &mouse;
    }

    const Mouse& Player::GetMouse() const
    {
        return *mouse;
    }

    void Player::SetShadowMapping(Rendering::ShadowMappingDemo& shadow)
    {
        shadowMapping = &shadow;
    }

    const Rendering::ShadowMappingDemo& Player::GetShadowMapping() const
    {
        return *shadowMapping;
    }

    void Player::Initialize()
    {
        currentPosition = Vector3Helper::Zero;
        rightVector = Vector3Helper::Right;
        forwardVector = Vector3Helper::Forward;
        upVector = Vector3Helper::Up;
    }

    void Player::Update(const GameTime& gameTime)
    {
        XMFLOAT3 movementAmount = Vector3Helper::Zero;

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

        XMFLOAT2 rotationAmount = Vector2Helper::Zero;


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

    void Player::ApplyRotation(float elapsedTime, XMFLOAT2 rotation) {
        if (rotation.x != 0 || rotation.y != 0) {
            XMVECTOR rotationVector = XMLoadFloat2(&rotation) * DefaultRotationRate * elapsedTime;
            XMVECTOR forward = XMLoadFloat3(&forwardVector);
            XMVECTOR right = XMLoadFloat3(&rightVector);
            XMVECTOR up = XMLoadFloat3(&upVector);
           
            XMMATRIX pitchMatrix =  XMMatrixRotationAxis(right, XMVectorGetY(rotationVector));
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
            XMStoreFloat3(&upVector, forward);
            XMStoreFloat3(&rightVector, right);
            shadowMapping->SetRotation(forwardVector, upVector, rightVector);
            camera->SetRotation(forwardVector, upVector, rightVector);
        }
    }

    void Player::Move(float elapsedTime, XMFLOAT3 movementAmount) {
        if (movementAmount.x != 0 || movementAmount.y != 0 || movementAmount.z != 0) {
            XMVECTOR position = XMLoadFloat3(&currentPosition);
            XMVECTOR movement = XMLoadFloat3(&movementAmount) * DefaultMovementRate * elapsedTime;

            auto mStraightDirection = forwardVector;
            mStraightDirection.y = 0;

            XMStoreFloat3(&mStraightDirection, XMVector3Normalize(XMLoadFloat3(&mStraightDirection)));
            XMVECTOR fixedRight;

            fixedRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&mStraightDirection), XMLoadFloat3(&Vector3Helper::Up)));


            XMVECTOR strafe = fixedRight * XMVectorGetX(movement);
            position += strafe;

            XMVECTOR forward = XMLoadFloat3(&mStraightDirection) * XMVectorGetZ(movement);
            position += forward;

            XMStoreFloat3(&currentPosition, position);
            camera->SetPosition(currentPosition);
            shadowMapping->SetPosition(currentPosition);
        }
    }

    void Player::ResetPosition(const XMFLOAT3 &position) {
        currentPosition = position;
        camera->SetPosition(position);
        shadowMapping->SetPosition(currentPosition);
    }
}