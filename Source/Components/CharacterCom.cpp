#include "CharacterCom.h"
#include "MovementCom.h"
#include "CameraCom.h"
#include "GameSource\Scene\SceneManager.h"
#include "Input\Input.h"

void CharacterCom::Update(float elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //入力値取得
    DirectX::XMFLOAT3 moveVec = CalcMoveVec();

    //移動ベクトルを設定
    //DirectX::XMFLOAT3 v = moveVec * moveCom->GetMoveAcceleration();
    //moveCom->SetVelocityX(v.x);
    //moveCom->SetVelocityZ(v.z);
}

DirectX::XMFLOAT3 CharacterCom::CalcMoveVec() const
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();
    CameraCom* cameraCom = cameraObj->GetComponent<CameraCom>().get();

    //入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& cameraRight = cameraCom->GetRight();
    const DirectX::XMFLOAT3& cameraFront = cameraCom->GetFront();

    //移動ベクトルはXZ平面

    //カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        //単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    //スティックの水平入力値をカメラ右方向に反映し、
    //スティックの垂直入力値をカメラ前方向に反映し、
    //進行ベクトルを計算する
    DirectX::XMFLOAT3 vec = {};
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.y = 0.0f;

    return vec;
}
