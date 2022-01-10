#include "Camera.h"
#include "Game.h"
#include "GameTime.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"

namespace Library
{
    RTTI_DEFINITIONS(Camera)

    const float Camera::DefaultFieldOfView = XM_PIDIV4;
    const float Camera::DefaultNearPlaneDistance = 0.01f;
    const float Camera::DefaultFarPlaneDistance = 1000.0f;

    Camera::Camera(Game& game)
        : GameComponent(game),
          mFieldOfView(DefaultFieldOfView), mAspectRatio(game.AspectRatio()), mNearPlaneDistance(DefaultNearPlaneDistance), mFarPlaneDistance(DefaultFarPlaneDistance),
          mPosition(), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
    {
    }

    Camera::Camera(Game& game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        : GameComponent(game),
          mFieldOfView(fieldOfView), mAspectRatio(aspectRatio), mNearPlaneDistance(nearPlaneDistance), mFarPlaneDistance(farPlaneDistance),
          mPosition(), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
    {
    }

    Camera::~Camera()
    {
    }
    
    const XMFLOAT3& Camera::Position() const
    {
        return currentPosition;
    }

    const XMFLOAT3& Camera::Direction() const
    {
        return forwardVector;
    }
    
    const XMFLOAT3& Camera::Up() const
    {
        return upVector;
    }

    const XMFLOAT3& Camera::Right() const
    {
        return rightVector;
    }

    XMVECTOR Camera::PositionVector() const
    {
        return XMLoadFloat3(&currentPosition);
    }

    XMVECTOR Camera::DirectionVector() const
    {
        return XMLoadFloat3(&forwardVector);
    }

    XMVECTOR Camera::UpVector() const
    {
        return XMLoadFloat3(&upVector);
    }
    
    XMVECTOR Camera::RightVector() const
    {
        return XMLoadFloat3(&rightVector);
    }

    float Camera::AspectRatio() const
    {
        return mAspectRatio;
    }

    float Camera::FieldOfView() const
    {
        return mFieldOfView;
    }

    float Camera::NearPlaneDistance() const
    {
        return mNearPlaneDistance;
    }

    float Camera::FarPlaneDistance() const
    {
        return mFarPlaneDistance;
    }

    XMMATRIX Camera::ViewMatrix() const
    {
        return XMLoadFloat4x4(&mViewMatrix);
    }

    XMMATRIX Camera::ProjectionMatrix() const
    {
        return XMLoadFloat4x4(&mProjectionMatrix);
    }

    XMMATRIX Camera::ViewProjectionMatrix() const
    {
        XMMATRIX viewMatrix = XMLoadFloat4x4(&mViewMatrix);
        XMMATRIX projectionMatrix = XMLoadFloat4x4(&mProjectionMatrix);

        return XMMatrixMultiply(viewMatrix, projectionMatrix);
    }

   /* void Camera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
    {
        XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
        SetPosition(position);
    }

    void Camera::SetPosition(FXMVECTOR position)
    {
        XMStoreFloat3(&mPosition, position);
    }*/

  //  void Camera::SetPosition(const XMFLOAT3& position)
  //  {
  //      mPosition = position;
  //  }

    void Camera::Reset()
    {
        currentPosition = Vector3Helper::Zero;
        forwardVector = Vector3Helper::Forward;
        upVector = Vector3Helper::Up;
        rightVector = Vector3Helper::Right;
        
        UpdateViewMatrix();
    }

	void Camera::Initialize()
    {
		UpdateProjectionMatrix();
        Reset();
    }

    void Camera::Update(const GameTime& gameTime)
    {
        UpdateViewMatrix();
    }

    void Camera::UpdateViewMatrix()
    {
        XMVECTOR eyePosition = XMLoadFloat3(&currentPosition);
        XMVECTOR direction = XMLoadFloat3(&forwardVector);
        XMVECTOR upDirection = XMLoadFloat3(&upVector);

        XMMATRIX viewMatrix = XMMatrixLookToRH(eyePosition, direction, upDirection);
        XMStoreFloat4x4(&mViewMatrix, viewMatrix);
    }

    void Camera::UpdateProjectionMatrix()
    {
        XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(mFieldOfView, mAspectRatio, mNearPlaneDistance, mFarPlaneDistance);
        XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
    }

    void Camera::ApplyRotation(CXMMATRIX transform)
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

    void Camera::ApplyRotation(const XMFLOAT4X4& transform)
    {
        XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
        ApplyRotation(transformMatrix);
    }
}
