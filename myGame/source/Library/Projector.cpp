#include "Projector.h"
#include "Game.h"
#include "GameTime.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"

namespace Library
{
    RTTI_DEFINITIONS(Projector)

    const float Projector::DefaultFieldOfView = XM_PIDIV4;
    const float Projector::DefaultAspectRatio = 4.0f / 3.0f;
	const float Projector::DefaultNearPlaneDistance = .5f;
    const float Projector::DefaultFarPlaneDistance = 100.0f;

    Projector::Projector(Game& game)
        : GameComponent(game),
          mFieldOfView(DefaultFieldOfView), mAspectRatio(DefaultAspectRatio), mNearPlaneDistance(DefaultNearPlaneDistance), mFarPlaneDistance(DefaultFarPlaneDistance),
          mPosition(), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
    {
    }

    Projector::Projector(Game& game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        : GameComponent(game),
          mFieldOfView(fieldOfView), mAspectRatio(aspectRatio), mNearPlaneDistance(nearPlaneDistance), mFarPlaneDistance(farPlaneDistance),
        mPosition(), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
    {
    }

    Projector::~Projector()
    {
    }
    
    const XMFLOAT3& Projector::Position() const
    {
        return currentPosition;
    }

    const XMFLOAT3& Projector::Direction() const
    {
        return forwardVector;
    }
    
    const XMFLOAT3& Projector::Up() const
    {
        return upVector;
    }

    const XMFLOAT3& Projector::Right() const
    {
        return rightVector;
    }

    XMVECTOR Projector::PositionVector() const
    {
        return XMLoadFloat3(&currentPosition);
    }

    XMVECTOR Projector::DirectionVector() const
    {
        return XMLoadFloat3(&forwardVector);
    }

    XMVECTOR Projector::UpVector() const
    {
        return XMLoadFloat3(&upVector);
    }
    
    XMVECTOR Projector::RightVector() const
    {
        return XMLoadFloat3(&rightVector);
    }

    float Projector::AspectRatio() const
    {
        return mAspectRatio;
    }

    float Projector::FieldOfView() const
    {
        return mFieldOfView;
    }

    float Projector::NearPlaneDistance() const
    {
        return mNearPlaneDistance;
    }

    float Projector::FarPlaneDistance() const
    {
        return mFarPlaneDistance;
    }

    XMMATRIX Projector::ViewMatrix() const
    {
        return XMLoadFloat4x4(&mViewMatrix);
    }

    XMMATRIX Projector::ProjectionMatrix() const
    {
        return XMLoadFloat4x4(&mProjectionMatrix);
    }

    XMMATRIX Projector::ViewProjectionMatrix() const
    {
        XMMATRIX viewMatrix = XMLoadFloat4x4(&mViewMatrix);
        XMMATRIX projectionMatrix = XMLoadFloat4x4(&mProjectionMatrix);

        return XMMatrixMultiply(viewMatrix, projectionMatrix);
    }

    void Projector::SetPosition(FLOAT x, FLOAT y, FLOAT z)
    {
        XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
        SetPosition(position);
    }

    void Projector::SetPosition(FXMVECTOR position)
    {
        XMStoreFloat3(&currentPosition, position);
    }

    void Projector::SetPosition(const XMFLOAT3& position)
    {
        currentPosition = position;
    }

    void Projector::Reset()
    {
        currentPosition = Vector3Helper::Zero;
        forwardVector = Vector3Helper::Forward;
        upVector = Vector3Helper::Up;
        rightVector = Vector3Helper::Right;
        
        UpdateViewMatrix();
    }

	void Projector::Initialize()
    {
		UpdateProjectionMatrix();
        Reset();
    }

    void Projector::Update(const GameTime& gameTime)
    {
        UpdateViewMatrix();
    }

    void Projector::UpdateViewMatrix()
    {
        XMVECTOR eyePosition = XMLoadFloat3(&currentPosition);
        XMVECTOR direction = XMLoadFloat3(&forwardVector);
        XMVECTOR upDirection = XMLoadFloat3(&upVector);

        XMMATRIX viewMatrix = XMMatrixLookToRH(eyePosition, direction, upDirection);
        XMStoreFloat4x4(&mViewMatrix, viewMatrix);
    }

    void Projector::UpdateProjectionMatrix()
    {
        XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(mFieldOfView, mAspectRatio, mNearPlaneDistance, mFarPlaneDistance);
        XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
    }

    void Projector::ApplyRotation(CXMMATRIX transform)
    {
        XMVECTOR direction = XMLoadFloat3(&forwardVector);
        XMVECTOR up = XMLoadFloat3(&upVector);
        
        direction = XMVector3TransformNormal(direction, transform);
        direction = XMVector3Normalize(direction);

        up = XMVector3TransformNormal(up, transform);
        up = XMVector3Normalize(up);

        XMVECTOR right = XMVector3Cross(direction, up);
        up = XMVector3Cross(right, direction);

        XMStoreFloat3(&forwardVector, direction);
        XMStoreFloat3(&upVector, up);
        XMStoreFloat3(&rightVector, right);
    }

    void Projector::ApplyRotation(const XMFLOAT4X4& transform)
    {
        XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
        ApplyRotation(transformMatrix);
    }
}
