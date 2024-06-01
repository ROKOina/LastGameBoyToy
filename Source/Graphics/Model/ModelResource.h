#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class ModelResource
{
public:
	ModelResource() {}
	virtual ~ModelResource() {}

	using NodeId = UINT64;

    //�m�[�h
    struct Node
    {
        NodeId				id;
        std::string			name;
        std::string			path;
        int					parentIndex;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;
        int                 layer = 0;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�}�e���A��
    struct Material
    {
        std::string			name;
        std::string			textureFilename[6];
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3   emissivecolor = { 1.0f,1.0f,1.0f };
        float               emissiveintensity = 0;
        float               Metalness = 1;
        float               Roughness = 1;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[6];

        void LoadTexture(ID3D11Device* device, const char* filename, int number);

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�T�u�Z�b�g
    struct Subset
    {
        UINT		startIndex = 0;
        UINT		indexCount = 0;
        int			materialIndex = 0;

        Material* material = nullptr;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //���_
    struct Vertex
    {
        DirectX::XMFLOAT3	position = { 0, 0, 0 };
        DirectX::XMFLOAT3	normal = { 0, 0, 0 };
        DirectX::XMFLOAT3	tangent = { 0, 0, 0 };
        DirectX::XMFLOAT2	texcoord = { 0, 0 };
        DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };
        DirectX::XMFLOAT4	boneWeight = { 1, 0, 0, 0 };
        DirectX::XMUINT4	boneIndex = { 0, 0, 0, 0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //���b�V��
    struct Mesh
    {
        std::vector<Vertex>						vertices;
        std::vector<UINT>						indices;
        std::vector<Subset>						subsets;

        int										nodeIndex;
        std::vector<int>						nodeIndices;
        std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;

        DirectX::XMFLOAT3						boundsMin;
        DirectX::XMFLOAT3						boundsMax;

        Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�m�[�h���
    struct NodeKeyData
    {
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�L�[�ӂ�[��
    struct Keyframe
    {
        float						seconds;
        std::vector<NodeKeyData>	nodeKeys;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�A�j���[�V�����C�x���g
    struct AnimationEvent
    {
        std::string name;
        float startframe;
        float endframe;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //���[�g�ʒu
    struct RootPosition
    {
        float frame;
        DirectX::XMFLOAT3 position;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //�A�j���[�V����
    struct Animation
    {
        std::string					name;
        float						secondsLength;
        float                       animationspeed = 1.0f;
        std::vector<Keyframe>		keyframes;
        std::vector<AnimationEvent>	animationevents;
        std::vector<RootPosition>   rootpositions;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    // �e��f�[�^�擾
	const std::vector<Mesh>& GetMeshes() const { return meshes_; }
	std::vector<Node>& GetNodes() { return nodes_; }
	const std::vector<Animation>& GetAnimations() const { return animations_; }
	const std::vector<Material>& GetMaterials() const { return materials_; }
	std::vector<Mesh>& GetMeshesEdit() { return meshes_; }
	std::vector<Material>& GetMaterialsEdit()  { return materials_; }

    // �ǂݍ���
	void Load(ID3D11Device* device, const char* filename);

protected:
	// ���f���Z�b�g�A�b�v
	void BuildModel(ID3D11Device* device, const char* dirname);

	// �e�N�X�`���ǂݍ���
	HRESULT LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color = 0xFFFFFFFF);

	// �V���A���C�Y
	void Serialize(const char* filename);

	// �f�V���A���C�Y
	void Deserialize(const char* filename);

    //�}�e���A���f�V���A���C�Y
    void MaterialDeserialize(const char* filename);

    //�A�j���[�V�����f�V���A���C�Y
    void AnimationDeserialize(const char* filename);

    //�m�[�h�f�V���A���C�Y
    void NodeDeserialize(const char* filename);
    
    // �m�[�h�C���f�b�N�X���擾����
	int FindNodeIndex(NodeId nodeId) const;

protected:
	std::vector<Node>		nodes_;
	std::vector<Material>	materials_;
	std::vector<Mesh>		meshes_;
	std::vector<Animation>	animations_;
};
