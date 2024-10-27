#include "DecalCom.h"
#include "Graphics\Graphics.h"
#include "Misc.h"

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
}

//描画
void Decal::Render()
{
}

//imgui
void Decal::OnGUI()
{
}