#include "DecalCom.h"
#include "Graphics\Graphics.h"
#include "SystemStruct\Misc.h"
#include "Graphics/Texture.h"

//コンストラクタ
Decal::Decal(const char* filename)
{
    HRESULT hr{ S_OK };

    DirectX::XMFLOAT3 vertices[24] = {};
    UINT indices[36] = {};

    int face = 0;

    // top-side
// 0---------1
// |         |
// |   -Y    |
// |         |
// 2---------3
    face = 0;
    vertices[face * 4 + 0] = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1] = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2] = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3] = { +0.5f, +0.5f, -0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // bottom-side
    // 0---------1
    // |         |
    // |   -Y    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0] = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1] = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2] = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3] = { +0.5f, -0.5f, -0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // front-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0] = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1] = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2] = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3] = { +0.5f, -0.5f, -0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // back-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0] = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1] = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2] = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3] = { +0.5f, -0.5f, +0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // right-side
    // 0---------1
    // |         |
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0] = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1] = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2] = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3] = { +0.5f, -0.5f, +0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // left-side
    // 0---------1
    // |         |
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0] = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1] = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2] = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3] = { -0.5f, -0.5f, +0.5f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    D3D11_BUFFER_DESC bufferdesc{};
    D3D11_SUBRESOURCE_DATA subresourcedata{};

    Graphics& graphics = Graphics::Instance();

    //vertexbuffer
    bufferdesc.ByteWidth = static_cast<UINT>(sizeof(vertices));
    bufferdesc.Usage = D3D11_USAGE_DEFAULT;
    bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferdesc.CPUAccessFlags = 0;
    bufferdesc.MiscFlags = 0;
    bufferdesc.StructureByteStride = 0;
    subresourcedata.pSysMem = vertices;
    subresourcedata.SysMemPitch = 0;
    subresourcedata.SysMemSlicePitch = 0;
    hr = graphics.GetDevice()->CreateBuffer(&bufferdesc, &subresourcedata, vertexbuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //indexbuffer
    bufferdesc.ByteWidth = static_cast<UINT>(sizeof(indices));
    bufferdesc.Usage = D3D11_USAGE_DEFAULT;
    bufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresourcedata.pSysMem = indices;
    hr = graphics.GetDevice()->CreateBuffer(&bufferdesc, &subresourcedata, indexbuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //ConstantBuffer
    DCB = std::make_unique<ConstantBuffer<DecalConstantBuffer>>(graphics.GetDevice());

    //頂点シェーダー
    //入力レイアウト
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        // 入力要素の設定
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(graphics.GetDevice(), "Shader\\DecalVS.cso", vertexshader.GetAddressOf(), inpulayout.GetAddressOf(), IED, ARRAYSIZE(IED));

    // ピクセルシェーダー
    CreatePsFromCso(graphics.GetDevice(), "Shader\\DecalPS.cso", pixelshader.GetAddressOf());

    //テクスチャ読み込み
    LoadTextureFromFile(Graphics::Instance().GetDevice(), filename, decalmap.GetAddressOf(), NULL);
}

//描画
void Decal::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cacheddepthstencilstate;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cachedrasterizerstate;
    dc->OMGetDepthStencilState(cacheddepthstencilstate.GetAddressOf(), 0);
    dc->RSGetState(cachedrasterizerstate.GetAddressOf());

    UINT stride = sizeof(DirectX::XMFLOAT3);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inpulayout.Get());

    for (decltype(spots)::const_reference spot : spots)
    {
        //行列更新して定数バッファの更新をする
        DirectX::XMVECTOR Z = DirectX::XMVector3Normalize(DirectX::XMVectorSet(-spot.normal.x, -spot.normal.y, -spot.normal.z, 0));
        DirectX::XMVECTOR Y = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0, 1, 0, 0));
        DirectX::XMVECTOR X = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Y, Z));
        Y = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Z, X));
        DirectX::XMMATRIX R = DirectX::XMMatrixIdentity();
        R.r[0] = X;
        R.r[1] = Y;
        R.r[2] = Z;
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(spot.scale, spot.scale, spot.scale);
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(spot.position.x, spot.position.y, spot.position.z);
        DirectX::XMMATRIX W = S * R * T;
        DirectX::XMStoreFloat4x4(&DCB->data.world, W);

        //デカール専用の逆プロジェクション行列
        DirectX::XMMATRIX V = DirectX::XMMatrixInverse(NULL, W);
        DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(1, 1, 0, 1);
        DirectX::XMStoreFloat4x4(&DCB->data.decalinverseprojection, V * P);

        //定数バッファの更新
        DCB->Activate(dc, (int)CB_INDEX::DECAL, true, true, false, false, false, false);

        //描画ステート設定
        dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 1);
        dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_FRONT));
        dc->VSSetShader(vertexshader.Get(), NULL, 0);
        dc->PSSetShader(NULL, NULL, 0);
        dc->DrawIndexed(36, 0, 0);

        //描画ステート設定
        dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 0);
        dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_BACK));
        dc->VSSetShader(vertexshader.Get(), NULL, 0);
        dc->PSSetShader(pixelshader.Get(), NULL, 0);
        dc->PSSetShaderResources(0, 1, decalmap.GetAddressOf());
        dc->DrawIndexed(36, 0, 0);
    }

    dc->OMSetDepthStencilState(cacheddepthstencilstate.Get(), 0);
    dc->RSSetState(cachedrasterizerstate.Get());
}

//imgui
DirectX::XMFLOAT3 pos = {};
void Decal::OnGUI()
{
    ImGui::Text((char*)u8"リソース");
    ImGui::Image(decalmap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    if (ImGui::Button("Create"))
    {
        pos.x += 0.5f;
        pos.y += 0.3f;
        pos.z += 0.1f;
        Add(pos, { 1,1,1 }, 5.0f);
    }
}