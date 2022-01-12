#include "SpotLight.h"
#include "VectorHelper.h"

namespace Library
{
	RTTI_DEFINITIONS(SpotLight)

	const float SpotLight::DefaultInnerAngle = 0.75f;
	const float SpotLight::DefaultOuterAngle = 0.25f;

	SpotLight::SpotLight(Game& game)
		: PointLight(game), mInnerAngle(DefaultInnerAngle), mOuterAngle(DefaultOuterAngle),
		  mDirection(Vector3Helper::Forward), mUp(Vector3Helper::Up), mRight(Vector3Helper::Right)
	{
	}

	SpotLight::~SpotLight()
	{
	}

	const XMFLOAT3& SpotLight::Direction() const
	{
		return forwardVector;
	}
	
	const XMFLOAT3& SpotLight::Up() const
	{
		return upVector;
	}

	const XMFLOAT3& SpotLight::Right() const
	{
		return rightVector;
	}

	XMVECTOR SpotLight::DirectionVector() const
	{
		return XMLoadFloat3(&forwardVector);
	}

	XMVECTOR SpotLight::UpVector() const
	{
		return XMLoadFloat3(&upVector);
	}
	
	XMVECTOR SpotLight::RightVector() const
	{
		return XMLoadFloat3(&rightVector);
	}

	float SpotLight::InnerAngle()
	{
		return mInnerAngle;
	}

	void SpotLight::SetInnerAngle(float value)
	{
		mInnerAngle = value;
	}

	float SpotLight::OuterAngle()
	{
		return mOuterAngle;
	}

	void SpotLight::SetOuterAngle(float value)
	{
		mOuterAngle = value;
	}

	void SpotLight::ApplyRotation(CXMMATRIX transform)
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

    void SpotLight::ApplyRotation(const XMFLOAT4X4& transform)
    {
        XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
        ApplyRotation(transformMatrix);
    }
}