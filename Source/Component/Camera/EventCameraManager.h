#pragma once

#include <string>
#include <vector>
#include <memory>
#include <DirectXMath.h>

class GameObject;

// イベントカメラの動きを保存
class EventCameraManager
{
public:
    EventCameraManager() {}
    ~EventCameraManager() {}

    // インスタンス取得
    static EventCameraManager& Instance()
    {
        static EventCameraManager instance;
        return instance;
    }

private:
    //シリアライズかんけい
    void Serialize();
    void Deserialize(const char* filename);
    void LoadDesirialize();

public:

    void EventCameraImGui();
    void EventUpdate(float elapsedTime);

    //カメラの位置情報保存
    struct ECTransform
    {
        float frame;            //フレーム
        DirectX::XMFLOAT3 pos;  //位置

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    //シリアライザ情報
    struct SaveEventCameraBuff
    {
        std::string cameraName;     //カメラオブジェの名前
        std::string focusObjName;   //注視点オブジェの名前
        std::vector<ECTransform> ECTra;

        //ver2
        bool isFocusLocal = false;  //注視点の子にするか

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

public:
    //イベント再生
    void PlayEventCamera(std::string eventName);

    bool GetIsPlayEvent() { return isEventPlay; }

private:
    //フォーカスポジションから見たカメラポジションを返す
    DirectX::XMFLOAT3 FocusFromCameraPos();

    //フォーカスポジションから見たワールドポジションを返す
    DirectX::XMFLOAT3 FocusFromWorldPos(DirectX::XMFLOAT3 pos);

    //再生関数(内部)
    void PlayCameraLerp();

private:
    SaveEventCameraBuff saveEventCameraBuff;

    //ゲームオブジェクト保存用
    std::weak_ptr<GameObject> cameraObj;
    std::weak_ptr<GameObject> focusObj;

    //再生関係
    bool isEventPlay = false;   //再生中か
    float timer;                //再生中の時間
};