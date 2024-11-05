#pragma once

#include <DirectXMath.h>

#include "Component\System\Component.h"

//カメラ
class CameraCom : public Component
{
    //コンポーネントオーバーライド
public:
    CameraCom(float fovY, float aspect, float nearZ, float farZ);
    ~CameraCom() {}

    // 名前取得
    const char* GetName() const override { return "Camera"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    //Cameraクラス
public:
    //指定方向を向く
    void SetLookAt(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up = { 0,1,0 });

    //パースペクティブ設定
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
    //視野角
    void SetFov(float fov);
    float GetFov();

    //ビュー行列取得
    const DirectX::XMFLOAT4X4& GetView() const { return view_; }

    //プロジェクション行列取得
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection_; }

    //描画範囲取得
    const DirectX::XMFLOAT2& GetScope()const { return scope_; }

    //注視点取得
    const DirectX::XMFLOAT3& GetFocus()const { return focus_; }

    //上方向取得
    const DirectX::XMFLOAT3& GetUp()const { return up_; }

    //カメラの場所取得
    const DirectX::XMFLOAT3& GetEye()const { return eye_; }

    //前方向取得
    const DirectX::XMFLOAT3& GetFront()const { return front_; }
    void SetFrontX(const float& x) { this->front_.x = x; }

    //右方向取得
    const DirectX::XMFLOAT3& GetRight()const { return right_; }

    void SetFocus(DirectX::XMFLOAT3 pos) { focus_ = pos; }

    //カメラシェイク実行
    void CameraShake(float power, float seconds) {
        shakePower_ = power;
        shakeSec_ = seconds;
    }

    //この関数でカメラをアクティブにする
    void ActiveCameraChange();

    bool GetIsActive() { return isActiveCamera; }

    //Ui配置用のbool取得
    bool GetIsUiCreate() { return isUiCreate; }

    //カメライベント再生中か
    void SetIsPlayEvent(bool flg) { isPlayEvent = flg; }
    bool GetIsPlayEvent() { return isPlayEvent; }

private:
    //アクティブカメラ変更処理
    void ChangeActiveProcess();

protected:

    //現在使うカメラ（カメラ複数あれば、一つしかONにならない）
    bool isActiveCamera = false;

    //カメラ継承したら使いたいのでここで書く
    DirectX::XMFLOAT3 focus_ = { 0,0,0 };
    DirectX::XMFLOAT3 eye_ = { 0,0,0 };

    //ラープの値
    float focuslapelate = 0.0f;
    float eyelaperate = 0.0f;

    //演出系
    //カメラシェイク
    float shakeSec_ = {};    //秒数
    float shakePower_ = {};  //強さ
    DirectX::XMFLOAT3 shakePos_ = {};

private:

    //座標系
    DirectX::XMFLOAT4X4 view_ = {};
    DirectX::XMFLOAT4X4 projection_ = {};
    DirectX::XMFLOAT2 scope_ = { 0,0 };

    DirectX::XMFLOAT3 up_ = { 0,1,0 };
    DirectX::XMFLOAT3 front_ = { 0,0,1 };
    DirectX::XMFLOAT3 right_ = { 1,0,0 };

    bool isLookAt_ = false;

    //Uiを配置する時にマウスが固定されないようにする
    bool isUiCreate = false;

    //カメライベント中に勝手にtrueになる
    bool isPlayEvent = false;

    float fov = 45;
};