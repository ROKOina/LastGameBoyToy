#include "SpriteCollisionCom.h"
#include "SystemStruct\Misc.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Math/easing.h"

SpriteCollisionCom::SpriteCollisionCom()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    LoadTextureFromFile(device, "Data\\Texture\\collsionbox.png", collsionshaderResourceView_.GetAddressOf(), &collisionTexture2ddesc_);
}

void SpriteCollisionCom::Start()
{
    spr = GetGameObject()->GetComponent<SpriteCom>();
}

void SpriteCollisionCom::Update(float elapsedTime)
{
    //当たり判定
    if (!ontriiger)return;

    if (cursorVsCollsionBox())
    {
        //当たった瞬間を記録
        if (hit == false)
        {
            for (auto& enter : hitEnter)
            {
                if (enter != nullptr)
                {
                    // 登録された関数を実行
                    enter();
                }
            }
        }

        hit = true;
    }
    else
    {
        // 離れた瞬間を記録
        if (hit == true)
        {
            for (auto& exit : hitExit)
            {
                if (exit != nullptr)
                {
                    // 登録された関数を実行
                    exit();
                }
            }
        }
        hit = false;
    }
}

void SpriteCollisionCom::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
#ifdef DEBUG
    // 当たり判定の可視化
    if (!drawcollsion || !ontriiger)return;

    DrawCollsionBox();
#endif // DEBUG
}

void SpriteCollisionCom::OnGUI()
{
    //ImGui::DragFloat2((char*)u8"位置オフセット値", &spc.collsionpositionoffset.x);
    //ImGui::DragFloat2((char*)u8"スケールオフセット値", &spc.collsionscaleoffset.x);
    ImGui::Checkbox((char*)u8"当たり判定描画", &drawcollsion);
}

void SpriteCollisionCom::DrawCollsionBox()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11Device* device = graphics.GetDevice();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    auto spriteCom = spr.lock();

    // スプライトと同じビューポートの設定
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    Vector2        position = { GetGameObject()->transform_->GetWorldPosition().x, GetGameObject()->transform_->GetWorldPosition().y };
    Vector2 scale = { GetGameObject()->transform_->GetScale().x + offsetScale.x, GetGameObject()->transform_->GetScale().y + offsetScale.y };
    float          angle = GetGameObject()->transform_->GetEulerRotation().z;
    const Vector2& texSize = spriteCom->spc.texSize;
    const Vector2& texPos = spriteCom->spc.texPos;
    const Vector2& pivot = spriteCom->spc.pivot;

    // ピボットの処理
    Vector2 drawPivot = ((texSize * scale) / texSize) * pivot;

    position -= drawPivot;

    // 描画範囲
    Vector2 extent = texSize * scale;

    // アフィン変換
    // スプライトを構成する４頂点のスクリーン座標を計算する
    Vector2 positions[] = {
        Vector2(position.x            , position.y),			    // 左上
        Vector2(position.x + extent.x , position.y),			    // 右上
        Vector2(position.x            , position.y + extent.y),	// 左下
        Vector2(position.x + extent.x , position.y + extent.y),	// 右下
    };

    // スプライトを構成する４頂点のテクスチャ座標を計算する
    Vector2 texcoords[] = {
        Vector2(texPos.x            , texPos.y),			        // 左上
        Vector2(texPos.x + texSize.x, texPos.y),			        // 右上
        Vector2(texPos.x            , texPos.y + texSize.y),	    // 左下
        Vector2(texPos.x + texSize.x, texPos.y + texSize.y),	    // 右下
    };

    // スプライトの中心で回転させるために４頂点の中心位置が
    // 原点(0, 0)になるように一旦頂点を移動させる。
    Vector2 offset = position + extent * (pivot / texSize);
    for (auto& p : positions)
    {
        p -= offset;
    }

    // 頂点を回転させる
    float theta = angle /** (DirectX::XM_PI / 180.0f)*/;	// 角度をラジアン(θ)に変換
    float c = cosf(theta);
    float s = sinf(theta);
    for (auto& p : positions)
    {
        DirectX::XMFLOAT2 r = p;
        p.x = c * r.x + -s * r.y;
        p.y = s * r.x + c * r.y;
    }

    // 回転のために移動させた頂点を元の位置に戻す
    for (auto& p : positions)
    {
        p += offset;
    }

    // スクリーン座標系からNDC座標系へ変換する。
    for (auto& p : positions)
    {
        p.x = 2.0f * p.x / viewport.Width - 1.0f;
        p.y = 1.0f - 2.0f * p.y / viewport.Height;
    }

    float texWidth = (float)spriteCom->texture2ddesc_.Width;
    float texHeight = (float)spriteCom->texture2ddesc_.Height;

    // 頂点バッファのデータを作成
    for (int i = 0; i < 4; ++i)
    {
        texcoords[i].x /= texWidth;
        texcoords[i].y /= texHeight;
    }


    // 当たり判定用の矩形を描画するための頂点
    SpriteCom::Vertex collisionVertices[]
    {
        { { positions[0].x, positions[0].y, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 0.0f, 0.0f } }, // 赤色で表示
        { { positions[1].x, positions[1].y, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 1.0f, 0.0f } },
        { { positions[2].x, positions[2].y, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 0.0f, 1.0f } },
        { { positions[3].x, positions[3].y, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 1.0f, 1.0f } },
    };

    // 当たり判定用の矩形を描画するための頂点バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(collisionVertices);
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = collisionVertices;
    ID3D11Buffer* collisionVertexBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, &collisionVertexBuffer);
    if (FAILED(hr)) {
        LOG("当たり判定用頂点バッファの作成に失敗しました。HRESULT: 0x%X", hr);
        return; // エラー処理
    }

    // 描画設定
    UINT stride{ sizeof(SpriteCom::Vertex) };
    UINT pOffset{ 0 };
    dc->IASetVertexBuffers(0, 1, &collisionVertexBuffer, &stride, &pOffset);
    dc->PSSetShaderResources(0, 1, collsionshaderResourceView_.GetAddressOf());

    // スプライトの矩形を描画
    dc->Draw(4, 0);

    // メモリの解放
    if (collisionVertexBuffer) {
        collisionVertexBuffer->Release();
        collisionVertexBuffer = nullptr;
    }
}

bool SpriteCollisionCom::cursorVsCollsionBox()
{
    //マウスの位置
    Mouse& mouse = Input::Instance().GetMouse();
    Vector2 mousePos = { static_cast<float>(mouse.GetPositionX()),static_cast<float>(mouse.GetPositionY())};
    auto spriteCom = spr.lock();
    Vector2 center = { spriteCom->spc.texSize.x / 2 ,spriteCom->spc.texSize.y / 2 };
    Vector2 pivot = { spriteCom->spc.pivot.x ,spriteCom->spc.pivot.y };
    Vector2 pivotToCenterVec = center - pivot;
    Vector2 pos = { GetGameObject()->transform_->GetWorldPosition().x ,GetGameObject()->transform_->GetWorldPosition().y };
    Vector2 centerPos = pos + Normalize(pivotToCenterVec) * pivotToCenterVec.Length();
    Vector2 scale = { GetGameObject()->transform_->GetScale().x , GetGameObject()->transform_->GetScale().y };

    float left  = centerPos.x - (center.x * scale.x);
    float Right = centerPos.x + (center.x * scale.x);
    float Down  = centerPos.y - (center.y * scale.y);
    float Up    = centerPos.y + (center.y * scale.y);

    //判定
    if (left > mousePos.x)return false;
    if (Right < mousePos.x)return false;
    if (Down > mousePos.y)return false;
    if (Up < mousePos.y)return false;

    return true;
}
