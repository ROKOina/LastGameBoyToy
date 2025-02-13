#include "VideoCom.h"

//コンストラクタ
Video::Video(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    //頂点シェーダーと入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(device, "Shader\\VideoVS.cso", m_vertexshader.GetAddressOf(), m_inputlayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

    // ピクセルシェーダー
    CreatePsFromCso(device, "Shader\\VideoPS.cso", m_pixelshader.GetAddressOf());

    //頂点バッファの作成
    DirectX::XMFLOAT4 white(1, 1, 1, 1);
    vtxs =
    {
      { 0,0,0, 0,1, white},
      { 0,1,0, 0,0, white},
      { 1,0,0, 1,1, white},
      { 0,1,0, 0,0, white},
      { 1,1,0, 1,0, white},
      { 1,0,0, 1,1, white},
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(vtxs.size() * sizeof(Vertex));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vtxs.data();
    HRESULT hr = device->CreateBuffer(&bd, &InitData, vertexbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //定数バッファの作成
    VC = std::make_unique<constantBufferH::ConstantBuffer<VideoConstants>>(device);

    VideoTexture::createAPI();
    video.create(filename);
}

//デストラクタ
Video::~Video()
{
    video.destroy();
    VideoTexture::destroyAPI();
}

//更新処理
void Video::Update(float elapsedTime)
{
    video.update(elapsedTime);
}

//描画
void Video::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

    // ステートの設定
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //シェーダセット
    dc->IASetInputLayout(m_inputlayout.Get());
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);

    //ビデオのシェーダリソース設定
    video.getTexture()->activate(0);

    //行列を代入
    VC->data.world = GetGameObject()->transform_->GetWorldTransform();

    //定数バッファの更新
    VC->Activate(dc, 1, true, false, false, false, false, false);

    //描画準備
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->Draw(static_cast<UINT>(vtxs.size()), 0);
}

//imgui
void Video::OnGUI()
{
    ImGui::Text((char*)u8"リソース");
    ImGui::Image(video.getTexture()->shader_resource_view, { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
}