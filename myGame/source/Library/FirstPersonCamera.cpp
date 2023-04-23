#include "FirstPersonCamera.h"
#include "Game.h"
#include "GameTime.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "VectorHelper.h"
#include <iostream>

namespace Library
{
    RTTI_DEFINITIONS(FirstPersonCamera)

    const float FirstPersonCamera::DefaultRotationRate = XMConvertToRadians(1.0f);
    const float FirstPersonCamera::DefaultMovementRate = 10.0f;
    const float FirstPersonCamera::DefaultMouseSensitivity = 100.0f;

    FirstPersonCamera::FirstPersonCamera(Game& game)
        : Camera(game), mKeyboard(nullptr), mMouse(nullptr), mPointLight(nullptr), 
          mMouseSensitivity(DefaultMouseSensitivity), mRotationRate(DefaultRotationRate), mMovementRate(DefaultMovementRate)
    {
    }

    FirstPersonCamera::FirstPersonCamera(Game& game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        : Camera(game, fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance), mKeyboard(nullptr), mMouse(nullptr), mPointLight(nullptr),
          mMouseSensitivity(DefaultMouseSensitivity), mRotationRate(DefaultRotationRate), mMovementRate(DefaultMovementRate)
          
    {
    }

    FirstPersonCamera::~FirstPersonCamera()
    {
        mKeyboard = nullptr;
        mMouse = nullptr;
        mPointLight = nullptr;
    }

    const Keyboard& FirstPersonCamera::GetKeyboard() const
    {
        return *mKeyboard;
    }

    void FirstPersonCamera::SetKeyboard(Keyboard& keyboard)
    {
        mKeyboard = &keyboard;
    }

    const Mouse& FirstPersonCamera::GetMouse() const
    {
        return *mMouse;
    }

    void FirstPersonCamera::SetMouse(Mouse& mouse)
    {
        mMouse = &mouse;
    }

    float&FirstPersonCamera:: MouseSensitivity()
    {
        return mMouseSensitivity;
    }


    float& FirstPersonCamera::RotationRate()
    {
        return mRotationRate;
    }

    float& FirstPersonCamera::MovementRate()
    {
        return mMovementRate;
    }

    void FirstPersonCamera::Initialize()
    {
        mKeyboard = (Keyboard*)mGame->Services().GetService(Keyboard::TypeIdClass());
        mMouse = (Mouse*)mGame->Services().GetService(Mouse::TypeIdClass());
       

        Camera::Initialize();
    }

    void FirstPersonCamera::Update(const GameTime& gameTime)
    {
        XMFLOAT3 movementAmount = Vector3Helper::Zero;
        //XMFLOAT3 movementAmount = Vector3Helper::Zero;

        if (mKeyboard != nullptr)
        {
            if (mKeyboard->IsKeyDown(DIK_W))
            {
                movementAmount.z = 1.0f;
            }

            if (mKeyboard->IsKeyDown(DIK_S))
            {
                movementAmount.z = -1.0f;
            }

            if (mKeyboard->IsKeyDown(DIK_A))
            {
                movementAmount.x = -1.0f;
            }

            if (mKeyboard->IsKeyDown(DIK_D))
            {
                movementAmount.x = 1.0f;
            }

            //Q and E
    /*		if (mKeyboard->IsKeyDown(DIK_Q))
            {
                movementAmount.z = 1.0f;
            }

            if (mKeyboard->IsKeyDown(DIK_E))
            {
                movementAmount.z = -1.0f;
            }*/
        }

        XMFLOAT2 rotationAmount = Vector2Helper::Zero;


        if ((mMouse != nullptr) && (mMouse->IsButtonHeldDown(MouseButtonsLeft)))
        {
            LPDIMOUSESTATE mouseState = mMouse->CurrentState();
            rotationAmount.x = -mouseState->lX * 100.0f;
            rotationAmount.y = -mouseState->lY * 100.0f;
        }

        float elapsedTime = (float)gameTime.ElapsedGameTime();
        XMVECTOR rotationVector = XMLoadFloat2(&rotationAmount) * mRotationRate * elapsedTime;
        XMVECTOR right = XMLoadFloat3(&rightVector);

        XMMATRIX pitchMatrix = XMMatrixRotationAxis(right, XMVectorGetY(rotationVector));
        XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));

        //ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));
        XMVECTOR position = XMLoadFloat3(&currentPosition);
        XMVECTOR movement = XMLoadFloat3(&movementAmount) * mMovementRate * elapsedTime;

        auto mStraightDirection = forwardVector;
        mStraightDirection.y = 0;

        XMStoreFloat3(&mStraightDirection, XMVector3Normalize(XMLoadFloat3(&mStraightDirection)));
        XMVECTOR fixedRight;

        fixedRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&mStraightDirection), XMLoadFloat3(&Vector3Helper::Up)));

        XMVECTOR strafe = fixedRight * XMVectorGetX(movement);
        position += strafe;

        XMVECTOR forward = XMLoadFloat3(&mStraightDirection) * XMVectorGetZ(movement);
        position += forward;

        //XMStoreFloat3(&mPosition, position);
        /*
        //mPointLight->ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));

        XMVECTOR position = XMLoadFloat3(&mPosition);

        //XMVECTOR movement = XMLoadFloat2(&movementAmount) * mMovementRate * elapsedTime;

        // Q and E
        XMVECTOR movement = XMLoadFloat2(&movementAmount) * mMovementRate * elapsedTime;
        //XMVECTOR upDown = XMLoadFloat3(&Vector3Helper::Up) * XMVectorGetZ(movement);
        //position += upDown;

        mStraightDirection = mDirection;
        mStraightDirection.z = 0;

        XMStoreFloat3(&mStraightDirection, XMVector3Normalize(XMLoadFloat3(&mStraightDirection)));
        XMVECTOR fixedRight;

        fixedRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&mStraightDirection), XMLoadFloat3(&Vector3Helper::Up)));
        XMVECTOR strafe = fixedRight * XMVectorGetX(movement);// *DirectX::CXMVECTOR(1, 1, 0);
        position += strafe;

        XMVECTOR forward = XMLoadFloat3(&mDirection) *  XMVectorGetY(movement);// XMLoadFloat3(&mDirection) * XMVectorGetY(movement);
        position += forward;

        XMStoreFloat3(&mPosition, position);
        */

        Camera::Update(gameTime);
    }
}
