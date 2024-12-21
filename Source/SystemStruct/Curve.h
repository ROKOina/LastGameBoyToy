#pragma once
#include <string>
#include <vector>

//============================================================================
// �O���t
//----------------------------------------------------------------------------
class Curve
{
public:

    Curve();
    Curve(int count);
    ~Curve() = default;

    // Component��OnPropertyGUI�����ŌĂ�
    bool ShowGraph(std::string guiname);

    // �t�@�C���̖��O(***.curve)
    void LoadCurve(std::string filename);

    // �w�肳�ꂽ���Ԃ̃J�[�u�̒l
    float Evaluate(float time, bool smooth = true);

    //�t�@�C���p�X�擾
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

    // �V���A���C�Y
    void Serialize(const char* filename, bool ascii);

    // �f�V���A���C�Y
    bool Deserialize(const char* filename);
};