#include "PointLight.h"
#include "VectorHelper.h"
#include "Game.h"
#include "GameTime.h"
#include "MatrixHelper.h"

namespace Library
{
	RTTI_DEFINITIONS(PointLight)

	const float PointLight::DefaultFieldOfView = XM_PIDIV4;
	const float PointLight::DefaultAspectRatio = 4.0f / 3.0f;
	const float PointLight::DefaultNearPlaneDistance = .5f;
	const float PointLight::DefaultFarPlaneDistance = 100.0f;

	const float PointLight::DefaultRadius = 10.0f;

	PointLight::PointLight(Game& game)
		: Light(game), mPosition(Vector3Helper::Zero), mFieldOfView(DefaultFieldOfView), mAspectRatio(DefaultAspectRatio), mNearPlaneDistance(DefaultNearPlaneDistance), mFarPlaneDistance(DefaultFarPlaneDistance),
		mRadius(DefaultRadius), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
	{
	}

	PointLight::~PointLight()
	{
	}

	XMFLOAT3& PointLight::Position()
	{
		return currentPosition;
	}

	const XMFLOAT3& PointLight::Direction() const
	{
		return forwardVector;
	}

	const XMFLOAT3& PointLight::Up() const
	{
		return upVector;
	}

	const XMFLOAT3& PointLight::Right() const
	{
		return rightVector;
	}

	XMVECTOR PointLight::PositionVector() const
	{
		return XMLoadFloat3(&currentPosition);
	}

	XMVECTOR PointLight::DirectionVector() const
	{
		return XMLoadFloat3(&forwardVector);
	}

	XMVECTOR PointLight::UpVector() const
	{
		return XMLoadFloat3(&upVector);
	}

	XMVECTOR PointLight::RightVector() const
	{
		return XMLoadFloat3(&rightVector);
	}

	XMMATRIX PointLight::ViewMatrix() const
	{
		return XMLoadFloat4x4(&mViewMatrix);
	}

	XMMATRIX PointLight::ProjectionMatrix() const
	{
		return XMLoadFloat4x4(&mProjectionMatrix);
	}

	XMMATRIX PointLight::ViewProjectionMatrix() const
	{
		XMMATRIX viewMatrix = XMLoadFloat4x4(&mViewMatrix);
		XMMATRIX projectionMatrix = XMLoadFloat4x4(&mProjectionMatrix);

		return XMMatrixMultiply(viewMatrix, projectionMatrix);
	}

	float PointLight::Radius() const
	{
		return mRadius;
	}

	void PointLight::SetPosition(FLOAT x, FLOAT y, FLOAT z)
    {
        XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
        SetPosition(position);
    }

    void PointLight::SetPosition(FXMVECTOR position)
    {
        XMStoreFloat3(&currentPosition, position);
    }

    void PointLight::SetPosition(const XMFLOAT3& position)
    {
        currentPosition = position;
    }

	void PointLight::SetRadius(float value)
	{
		mRadius = value;
	}


	void PointLight::Update(const GameTime& gameTime)
	{
		UpdateViewMatrix();
	}

	void PointLight::UpdateViewMatrix()
	{
		XMVECTOR eyePosition = XMLoadFloat3(&currentPosition);
		XMVECTOR direction = XMLoadFloat3(&forwardVector);
		XMVECTOR upDirection = XMLoadFloat3(&upVector);

		XMMATRIX viewMatrix = XMMatrixLookToRH(eyePosition, direction, upDirection);
		XMStoreFloat4x4(&mViewMatrix, viewMatrix);
	}

	void PointLight::UpdateProjectionMatrix()
	{
		XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(mFieldOfView, mAspectRatio, mNearPlaneDistance, mFarPlaneDistance);
		XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
	}



	void PointLight::ApplyRotation(CXMMATRIX transform)
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

	void PointLight::ApplyRotation(const XMFLOAT4X4& transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		ApplyRotation(transformMatrix);
	}
}