#pragma once

class UiMove : public Component
{
public:
    UiMove() {};
    ~UiMove() {}

    // 名前取得
    const char* GetName() const override { return "UiMove"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection) {};

    // GUI描画
    void OnGUI() override;

public:
    struct SlideData
    {
        std::shared_ptr<bool> slideFlag;

        DirectX::XMFLOAT3 origin = {};
        DirectX::XMFLOAT3 target = {};

        float slideTime = 0;
        float slideTimer = 0;

        DirectX::XMFLOAT3 slideValue = {};
    };

    enum class SlideType
    {
        Pos,
        Rote,
        Scale,

        Max,
    };

    void SlideStart(SlideType type, bool isBack, float time);
    void SlideAllReset();
    void SlideReset(SlideType type);

    DirectX::XMFLOAT3 GetOriginValue(SlideType type) { return slideDatas[(int)type].origin; }
    DirectX::XMFLOAT3 GetTargetValue(SlideType type) { return slideDatas[(int)type].target; }
    void SetOriginValue(SlideType type, DirectX::XMFLOAT3 value) { slideDatas[(int)type].origin = value; }
    void SetTargetValue(SlideType type, DirectX::XMFLOAT3 value) { slideDatas[(int)type].target = value; }
    float GetSlideTimer(SlideType type) { return slideDatas[(int)type].slideTimer; }
    float GetSlideTime(SlideType type) { return slideDatas[(int)type].slideTime; }

    //動きの進行度取得
    float GetSlideProgress(SlideType type);

private:
    SlideData slideDatas[(int)SlideType::Max];
    void UseData(SlideType type);
};
