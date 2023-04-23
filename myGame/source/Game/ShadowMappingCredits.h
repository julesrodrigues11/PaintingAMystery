#pragma once

#include "DrawableGameComponent.h"
#include "Frustum.h"
#include "RenderStateHelper.h"
#include "SpotLight.h"
#include "Camera.h"
#include "ShadowMappingBase.h"

using namespace Library;

namespace Library
{
	class Effect;
	class Keyboard;
	class Mouse;
	class ProxyModel;
	class Projector;
	class RenderableFrustum;
	class ShadowMappingMaterial;
	class DepthMapMaterial;
	class DepthMap;
}

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace Rendering
{
	class ShadowMappingCredits : public ShadowMappingBase
	{
		RTTI_DECLARATIONS(ShadowMappingCredits, DrawableGameComponent)

	public:
		virtual void Initialize() override;
		ShadowMappingCredits(Library::Game& game, Library::Camera& camera);
	};
}

