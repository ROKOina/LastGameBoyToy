#pragma once
#include "System\Component.h"
#include "Components\RendererCom.h"
class FootIKCom : public Component
{
public:
    enum class LegNodes{
        RIGHT_WAIST,   //腰 
        RIGHT_KNEES,   //膝
        RIGHT_ANKLE,   //足首
        LEFT_WAIST,         
        LEFT_KNEES,
        LEFT_ANKLE,
        Max,
    };

    enum class Legs {
        RIGHT,    //ノードのレイヤーを参照するので3から
        LEFT,
        Max,
    };

public:
    //コンポーネントオーバーライド
    FootIKCom() {}
    ~FootIKCom() {}

    // 名前取得
    const char* GetName() const override { return "FootIK"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    

public:
    //ターゲットポジション取得
    bool GetTargetPosition(Legs leg, DirectX::XMFLOAT3& resultPos);
    //セッター
    void SetLegNodes() {};

    //ボーンの計算
    void MoveBone(Legs leg);

    //ボーンのワールド行列更新
    void UpdateWorldTransform(Model::Node* legNode);
private:
    //足ノードを保持
    Model::Node* legNodes[(int)LegNodes::Max] = {};
    DirectX::XMFLOAT3* rotate[(int)LegNodes::Max] = {};

    //ターゲットポジション
    DirectX::XMFLOAT3 targetPos[(int)Legs::Max] = {};

    //ターゲットポール
    DirectX::XMFLOAT4X4 poleLocalTransform[(int)Legs::Max] = {};
    DirectX::XMFLOAT4X4 poleWorldTransform[(int)Legs::Max] = {};
    
    //StageModel
    Model*  stageModel;
};