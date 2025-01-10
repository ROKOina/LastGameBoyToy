#pragma once
#include <string>
#include <vector>

//============================================================================
// グラフ
//----------------------------------------------------------------------------
class Curve
{
public:

    Curve();
    Curve(int count);
    ~Curve() = default;

    // ComponentのOnPropertyGUI内等で呼ぶ
    bool ShowGraph(std::string guiname);

    // ファイルの名前(***.curve)
    void LoadCurve(std::string filename);

    // 指定された時間のカーブの値
    float Evaluate(float time, bool smooth = true);

    //ファイルパス取得
    const std::string& GetFilePath() const { return filepath; }

private:

    std::vector<float> frames;
    std::vector<float> values;

    float curvePoint = 0.0f;
    int selectedPoint = -1;
    bool fitCurveInEditor = false;
    bool isSelectable = false;
    bool isHovered = false;

    std::string filepath = {};

private:

    // シリアライズ
    void Serialize(const char* filename, bool ascii);

    // デシリアライズ
    bool Deserialize(const char* filename);
};