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

    //ノード
    struct Node
    {
        NodeId				id;
        std::string			name;
        std::string			path;
        int					parentIndex;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;
        std::vector<int>    layer;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //マテリアル
    struct Material
    {
        std::string			name;
        std::string			textureFilename[6];
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3   emissivecolor = { 1.0f,1.0f,1.0f };
        float               emissiveintensity = 0;
        float               Metalness = 1;
        float               Roughness = 1;

        //オブジェクト毎の定数バッファをメッシュに入れておく(シリアライズは絶対する)
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
        float alpha = 1.0f;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[6];

        void LoadTexture(ID3D11Device* device, const char* filename, int number);

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //サブセット
    struct Subset
    {
        UINT		startIndex = 0;
        UINT		indexCount = 0;
        int			materialIndex = 0;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //頂点
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

    //メッシュ
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

    //ノード情報
    struct NodeKeyData
    {
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //キーフレーム
    struct Keyframe
    {
        float						seconds;
        std::vector<NodeKeyData>	nodeKeys;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //アニメーションイベント
    struct AnimationEvent
    {
        std::string name;
        float startframe;
        float endframe;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //ルート位置
    struct RootPosition
    {
        float frame;
        DirectX::XMFLOAT3 position;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //アニメーション
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

    // 各種データ取得
    const std::vector<Mesh>& GetMeshes() const { return meshes_; }
    std::vector<Node>& GetNodes() { return nodes_; }
    const std::vector<Animation>& GetAnimations() const { return animations_; }
    const std::vector<Material>& GetMaterials() const { return materials_; }
    std::vector<Mesh>& GetMeshesEdit() { return meshes_; }
    std::vector<Material>& GetMaterialsEdit() { return materials_; }
    std::string GetFileName() { return fileName; }

    // 読み込み
    void Load(ID3D11Device* device, const char* filename);
    // マテリアル情報のみ読み込む
    static void LoadMaterial(ID3D11Device* device, const char* filename, std::shared_ptr<std::vector<ModelResource::Material>> desc);

#ifdef _DEBUG
    //マテリアルシリアライズ
    void ModelResource::MaterialSerialize(const char* filename);

#endif // _DEBUG

    //上書きセーブ
    void AnimSerialize();

protected:
    // モデルセットアップ
    void BuildModel(ID3D11Device* device, const char* dirname);

    // シリアライズ
    void Serialize(const char* filename);

    // デシリアライズ
    void Deserialize(const char* filename);

    //マテリアルデシリアライズ
    static void MaterialDeserialize(const char* filename, std::vector<Material>& materials);

    //アニメーションデシリアライズ
    void AnimationDeserialize(const char* filename);

    //ノードデシリアライズ
    void NodeDeserialize(const char* filename);

    // ノードインデックスを取得する
    int FindNodeIndex(NodeId nodeId) const;

protected:
    std::vector<Node>		nodes_;
    std::vector<Material>	materials_;
    std::vector<Mesh>		meshes_;
    std::vector<Animation>	animations_;
    std::string fileName;
};