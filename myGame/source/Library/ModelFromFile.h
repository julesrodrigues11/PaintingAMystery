#pragma once

#include "DrawableGameComponent.h"

#include <DirectXCollision.h>

using namespace Library;

namespace Library
{
	class Mesh;
	class ModelFromFile : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(ModelFromFile, DrawableGameComponent)

	public:
		ModelFromFile(Game& game, Camera& camera, const std::string modelFilename, const std::wstring textureFilename, const std::wstring modelDes, bool isPainting);
		ModelFromFile(Game& game, Camera& camera, const std::string modelFilename, const std::wstring textureFilename, const std::wstring modelDes, bool isPainting, int keyID);
		~ModelFromFile();

		virtual void Update(const GameTime& gameTime) override;


		//add to support multiple model in the scene, remove this function

		void SetPosition(const float rotateX, const float rotateY, const float rotateZ, const float scaleFactor, const float translateX, const float translateY, const float translateZ);
		void SetPosition(const float translateX, const float translateY, const float translateZ);
		void ResetToOrigin();
		void SetOriginAndPosition(const float rotateX, const float rotateY, const float rotateZ, const float scaleFactor, const float translateX, const float translateY, const float translateZ);
		bool blocksMovement = false;
		
		//bounding box require to access the world matrix

		XMFLOAT4X4* WorldMatrix() { return &mWorldMatrix; }

		//need to access this , make this public for simplicity
		DirectX::BoundingBox mBoundingBox;
		const std::wstring GetModelDes() { return modelDes; }
		int const KeyID() { return keyID; }


		virtual void Initialize() override;
		virtual void Draw(const GameTime& gameTime) override;

		bool Taken();
		void Take();
		void Release();

		bool isPainting() { return painting; }

	private:
		std::vector<float> origin;
		float rotateX = 0.0f;
		float rotateY = 0.0f;
		float rotateZ = 0.0f;
		float scaleFactor = 0.0f;
		float translateX = 0.0f;
		float translateY = 0.0f;
		float translateZ = 0.0f;
		bool taken;
		typedef struct _TextureMappingVertex
		{
			XMFLOAT4 Position;
			XMFLOAT2 TextureCoordinates;

			_TextureMappingVertex() { }

			_TextureMappingVertex(XMFLOAT4 position, XMFLOAT2 textureCoordinates)
				: Position(position), TextureCoordinates(textureCoordinates) { }
		} TextureMappingVertex;

		ModelFromFile();
		ModelFromFile(const ModelFromFile& rhs);
		ModelFromFile& operator=(const ModelFromFile& rhs);

		void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const;

		ID3DX11Effect* mEffect;
		ID3DX11EffectTechnique* mTechnique;
		ID3DX11EffectPass* mPass;
		ID3DX11EffectMatrixVariable* mWvpVariable;

		ID3D11ShaderResourceView* mTextureShaderResourceView;
		ID3DX11EffectShaderResourceVariable* mColorTextureVariable;

		ID3D11InputLayout* mInputLayout;
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		UINT mIndexCount;

		XMFLOAT4X4 mWorldMatrix;
		float mAngle;

		const std::string modelFile;
		const std::wstring textureFile;

		std::wstring modelDes;
		int keyID;
		bool painting;

	};
}
