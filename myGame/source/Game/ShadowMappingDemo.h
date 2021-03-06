#pragma once

#include "DrawableGameComponent.h"
#include "Frustum.h"
#include "RenderStateHelper.h"
#include "SpotLight.h"
#include "Camera.h"

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
	enum ShadowMappingTechnique
	{
		ShadowMappingTechniqueSimple = 0,
		ShadowMappingTechniqueManualPCF,
		ShadowMappingTechniquePCF,
		ShadowMappingTechniqueEnd
	};

	const std::string ShadowMappingTechniqueNames[] = { "shadow_mapping", "shadow_mapping_manual_pcf", "shadow_mapping_pcf" };
	const std::string ShadowMappingDisplayNames[] = { "Shadow Mapping Simple", "Shadow Mapping w/ Manual PCF", "Shadow Mapping w/ PCF" };
	const std::string DepthMappingTechniqueNames[] = { "create_depthmap", "create_depthmap", "create_depthmap_w_bias", };

	class ShadowMappingDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(ShadowMappingDemo, DrawableGameComponent)

	public:
		ShadowMappingDemo(Game& game, Camera& camera);
		~ShadowMappingDemo();

		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;
		void ApplyRotation(XMMATRIX rotationMatrix);
		void SetRotation(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right);
		void SetPosition(XMFLOAT3 newPosition);
		XMFLOAT2 mousePosition;
		Light* GetLight();


	private:
		ShadowMappingDemo();
		ShadowMappingDemo(const ShadowMappingDemo& rhs);
		ShadowMappingDemo& operator=(const ShadowMappingDemo& rhs);

		void UpdateTechnique();
		void UpdateDepthBias(const GameTime& gameTime);
		void UpdateDepthBiasState();
		void UpdateAmbientLight(const GameTime& gameTime);
		void UpdatePointLightAndProjector(const GameTime& gameTime);
		void UpdateSpecularLight(const GameTime& gameTime);
		void InitializeProjectedTextureScalingMatrix();

		static const float LightModulationRate;
		static const float LightMovementRate;
		static const float LightRotationRate;
		static const UINT DepthMapWidth;
		static const UINT DepthMapHeight;
		static const RECT DepthMapDestinationRectangle;
		static const float DepthBiasModulationRate;

		Keyboard* mKeyboard;
		Mouse* mMouse;
		XMCOLOR mAmbientColor;
		
		XMCOLOR mSpecularColor;
		float mSpecularPower;
		ProxyModel* mProxyModel;
		RenderStateHelper mRenderStateHelper;
		Projector* mProjector;
		PointLight* mPointLight;
		Frustum mProjectorFrustum;
		RenderableFrustum* mRenderableProjectorFrustum;

		ID3D11Buffer* mPlanePositionVertexBuffer;
		ID3D11Buffer* mPlanePositionUVNormalVertexBuffer;
		ID3D11Buffer* mPlaneIndexBuffer;
		UINT mPlaneVertexCount;
		XMFLOAT4X4 mPlaneWorldMatrix;
		ID3D11ShaderResourceView* mCheckerboardTexture;

		ID3D11ShaderResourceView* mFloorTexture;



		Effect* mShadowMappingEffect;
		ShadowMappingMaterial* mShadowMappingMaterial;
		ID3D11Buffer* mModelPositionVertexBuffer;
		ID3D11Buffer* mModelPositionUVNormalVertexBuffer;
		ID3D11Buffer* mModelIndexBuffer;
		UINT mModelIndexCount;
		XMFLOAT4X4 mModelWorldMatrix;
		XMFLOAT4X4 mProjectedTextureScalingMatrix;
		
		Effect* mDepthMapEffect;
		DepthMapMaterial* mDepthMapMaterial;
		DepthMap* mDepthMap;
		bool mDrawDepthMap;
		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;
		ShadowMappingTechnique mActiveTechnique;
		XMFLOAT2 mTextPosition;
		ID3D11RasterizerState* mDepthBiasState;
		float mDepthBias;
		float mSlopeScaledDepthBias;
	};
}
