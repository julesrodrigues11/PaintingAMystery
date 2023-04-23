#include "ShadowMappingCredits.h"
#include "Game.h"
#include "GameException.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Utility.h"
#include "PointLight.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <WICTextureLoader.h>
#include "ProxyModel.h"
#include "Projector.h"
#include "RenderableFrustum.h"
#include "ShadowMappingMaterial.h"
#include "DepthMapMaterial.h"
#include "DepthMap.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <sstream>
#include <iomanip>
#include "FirstPersonCamera.h"

namespace Rendering
{
	RTTI_DEFINITIONS(ShadowMappingCredits)



		void ShadowMappingCredits::Initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		// Initialize materials
		mShadowMappingEffect = new Effect(*mGame);
		mShadowMappingEffect->LoadCompiledEffect(L"Content\\Effects\\ShadowMapping.cso");

		mShadowMappingMaterial = new ShadowMappingMaterial();
		mShadowMappingMaterial->Initialize(*mShadowMappingEffect);

		mDepthMapEffect = new Effect(*mGame);
		mDepthMapEffect->LoadCompiledEffect(L"Content\\Effects\\DepthMap.cso");

		mDepthMapMaterial = new DepthMapMaterial();
		mDepthMapMaterial->Initialize(*mDepthMapEffect);

		// Plane vertex buffers
		VertexPositionTextureNormal positionUVNormalVertices[] =
		{
			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),

			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), Vector3Helper::Backward),
		};

		mPlaneVertexCount = ARRAYSIZE(positionUVNormalVertices);
		std::vector<VertexPositionNormal> positionNormalVertices;
		positionNormalVertices.reserve(mPlaneVertexCount);
		std::vector<VertexPosition> positionVertices;
		positionVertices.reserve(mPlaneVertexCount);
		for (UINT i = 0; i < mPlaneVertexCount; i++)
		{
			positionNormalVertices.push_back(VertexPositionNormal(positionUVNormalVertices[i].Position, positionUVNormalVertices[i].Normal));
			positionVertices.push_back(VertexPosition(positionUVNormalVertices[i].Position));
		}

		mDepthMapMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), &positionVertices[0], mPlaneVertexCount, &mPlanePositionVertexBuffer);
		mShadowMappingMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), positionUVNormalVertices, mPlaneVertexCount, &mPlanePositionUVNormalVertexBuffer);



		//	std::wstring textureName = L"content\\Textures\\environment_texture.png";
	//		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mCheckerboardTexture);
	//		if (FAILED(hr))
	//		{
	//			throw GameException("CreateWICTextureFromFile() failed.", hr);
	//		}


			//create the floor texture

		std::wstring textureName = L"content\\Textures\\credits.png";
		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mFloorTexture);
		if (FAILED(hr))
		{
			throw GameException("CreateWICTextureFromFile() failed.", hr);
		}


		mPointLight = new PointLight(*mGame);
		mPointLight->SetRadius(25.0f);
		mPointLight->SetPosition(mCamera->Position());


		mKeyboard = (Keyboard*)mGame->Services().GetService(Keyboard::TypeIdClass());
		assert(mKeyboard != nullptr);

		mMouse = (Mouse*)mGame->Services().GetService(Mouse::TypeIdClass());

		mProxyModel = new ProxyModel(*mGame, *mCamera, "content\\Models\\PointLightProxy.obj", 0.5f);
		mProxyModel->Initialize();

		XMStoreFloat4x4(&mPlaneWorldMatrix, XMMatrixRotationX(0.0f) * XMMatrixRotationY(0.0f) * XMMatrixRotationZ(0.0f) * XMMatrixScaling(32.0f, 18.0f, 0.0f) * XMMatrixTranslation(0.0f, 4.5f, -20.5f));

		mProjector = new Projector(*mGame);
		mProjector->Initialize();

		mProjectorFrustum.SetMatrix(mProjector->ViewProjectionMatrix());

		//mRenderableProjectorFrustum = new RenderableFrustum(*mGame, *mCamera);
		//mRenderableProjectorFrustum->Initialize();
		//mRenderableProjectorFrustum->InitializeGeometry(mProjectorFrustum);

		InitializeProjectedTextureScalingMatrix();

		// Vertex and index buffers for a second model to render
		std::unique_ptr<Model> model(new Model(*mGame, "content\\Models\\environment.fbx", true));

		/*Mesh* mesh = model->Meshes().at(0);
		mDepthMapMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, &mModelPositionVertexBuffer);
		mShadowMappingMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, &mModelPositionUVNormalVertexBuffer);
		mesh->CreateIndexBuffer(&mModelIndexBuffer);
		mModelIndexCount = mesh->Indices().size();

		XMStoreFloat4x4(&mModelWorldMatrix, XMMatrixRotationX(0.0f) * XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 4.25f, -4.5f));

		*/


		mDepthMap = new DepthMap(*mGame, DepthMapWidth, DepthMapHeight);
		mSpriteBatch = new SpriteBatch(mGame->Direct3DDeviceContext());
		mSpriteFont = new SpriteFont(mGame->Direct3DDevice(), L"content\\Fonts\\Arial_14_Regular.spritefont");

		UpdateDepthBiasState();
	}

	ShadowMappingCredits::ShadowMappingCredits(Library::Game& game, Library::Camera& camera) : ShadowMappingBase(game, camera) {}

}