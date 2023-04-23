#pragma once

#include "DrawableGameComponent.h"

#include <DirectXCollision.h>

using namespace Library;

namespace Library
{
	class Mesh;
	class Door : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(Door, DrawableGameComponent)

	public:
		Door(Game& game, Camera& camera, const std::string modelFilename, const std::wstring textureFilename, int id, bool secret);

		Door(Game& game, Camera& camera, const std::string modelFilename, const std::wstring textureFilename, int id, bool secret, const std::wstring ModelDes);

		Door(Game& game, Camera& camera, const std::string modelFilename, const std::wstring textureFilename, int id, bool secret, const std::wstring ModelDes, Door& door);

		~Door();

		virtual void Update(const GameTime& gameTime) override;

		void SetPosition(const float rotateX, const float rotateY, const float rotateZ, const float scaleFactor, const float translateX, const float translateY, const float translateZ);

		XMFLOAT4X4* WorldMatrix() { return &mWorldMatrix; }

		DirectX::BoundingBox mBoundingBox;
		const std::wstring GetModelDes() { return modelDes; }

		virtual void Initialize() override;
		virtual void Draw(const GameTime& gameTime) override;

		bool getLocked() { return Locked; }
		void setLocked(bool lock);

		int getID() { return DoorID; }

		bool isNumpad() { return numpad; }
		bool isSecret() { return IsSecret; }

	private:
		float rotateX = 0.0f;
		float rotateY = 0.0f;
		float rotateZ = 0.0f;
		float scaleFactor = 0.0f;
		float translateX = 0.0f;
		float translateY = 0.0f;
		float translateZ = 0.0f;

		typedef struct _TextureMappingVertex
		{
			XMFLOAT4 Position;
			XMFLOAT2 TextureCoordinates;

			_TextureMappingVertex();
			_TextureMappingVertex(XMFLOAT4 position, XMFLOAT2 textureCoordinates)
				:Position(position), TextureCoordinates(textureCoordinates) {}
		} TextureMappingVertex;

		Door();
		Door(const Door& rhs);
		Door& operator=(const Door& rhs);

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

		int DoorID;
		bool Locked;
		bool numpad;
		bool IsSecret;
		Door* secretDoor;
	};
}