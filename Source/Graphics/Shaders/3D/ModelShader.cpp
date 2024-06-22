#include "ModelShader.h"
#include "Graphics/Shaders/Texture.h"

//初期化
ModelShader::ModelShader(int shader)
{
    Graphics& Graphics = Graphics::Instance();

    //ファイル名
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;

    //選択されたのが指定される
    switch (shader)
    {
    case MODELSHADER::DEFALT:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\DefaltPS.cso" };
        break;
    case MODELSHADER::DEFERRED:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\PBR+IBL_Unity.cso" };
        break;
    case MODELSHADER::BLACK:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\BlackPS.cso" };
        break;
    case MODELSHADER::AREA_EFFECT_CIRCLE:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\AreaEffectCirclePS.cso" };
        break;
    }

    // 頂点シェーダー
    //入力レイアウト
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        // 入力要素の設定
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));

    // ピクセルシェーダー
    CreatePsFromCso(Graphics.GetDevice(), PSPath, m_pixelshader.GetAddressOf());

    // 定数バッファ
    {
        // オブジェクト用のコンスタントバッファ,サブセット用
        m_objectconstants = std::make_unique<ConstantBuffer<objectconstants>>(Graphics.GetDevice());
        m_subsetconstants = std::make_unique<ConstantBuffer<subsetconstants>>(Graphics.GetDevice());
    }

    //IBL専用のテクスチャ読み込み
    {
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics.GetDevice(), "Data\\Texture\\snowy_hillside_4k.DDS", m_skybox.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), "Data\\Texture\\diffuse_iem.dds", m_diffuseiem.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), "Data\\Texture\\specular_pmrem.dds", m_specularpmrem.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), "Data\\Texture\\lut_ggx.DDS", m_lutggx.GetAddressOf(), &texture2d_desc);
    }
}

//描画設定
void ModelShader::Begin(ID3D11DeviceContext* dc, int blendmode)
{
    //シェーダーのセット
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //レンダーステートの設定（ループの外で1回だけ設定）
    dc->OMSetBlendState(Graphics.GetBlendState(static_cast<BLENDSTATE>(blendmode)), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_BACK));

    //IBL専用のテクスチャセット
    dc->PSSetShaderResources(6, 1, m_skybox.GetAddressOf());
    dc->PSSetShaderResources(7, 1, m_diffuseiem.GetAddressOf());
    dc->PSSetShaderResources(8, 1, m_specularpmrem.GetAddressOf());
    dc->PSSetShaderResources(9, 1, m_lutggx.GetAddressOf());
}

//バッファー描画
void ModelShader::SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
    if (mesh.nodeIndices.size() > 0)
    {
        for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
        {
            DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
            DirectX::XMMATRIX boneTransform = offsetTransform * DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
            DirectX::XMStoreFloat4x4(&m_objectconstants->data.BoneTransforms[i], boneTransform);
        }
    }
    else
    {
        DirectX::XMStoreFloat4x4(&m_objectconstants->data.BoneTransforms[0], DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform));
    }

    //コンスタントバッファの更新
    m_objectconstants->Activate(dc, 1, true, true, false, false, false, false);

    UINT stride = sizeof(ModelResource::Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//サブセット毎の描画
void ModelShader::SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    //コンスタントバッファの情報
    m_subsetconstants->data.color = subset.material->color;
    m_subsetconstants->data.emissivecolor = subset.material->emissivecolor;
    m_subsetconstants->data.emissiveintensity = subset.material->emissiveintensity;
    m_subsetconstants->data.Metalness = subset.material->Metalness;
    m_subsetconstants->data.Roughness = subset.material->Roughness;
    m_subsetconstants->Activate(dc, 2, true, true, false, false, false, false);

    //シェーダーリソースビュー設定
    dc->PSSetShaderResources(0, std::size(subset.material->shaderResourceView), subset.material->shaderResourceView[0].GetAddressOf());

    dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
}

//描画終了処理
void ModelShader::End(ID3D11DeviceContext* dc)
{
    dc->IASetInputLayout(nullptr);

    //解放してあげる
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
}