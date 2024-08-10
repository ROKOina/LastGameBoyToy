#pragma warning (disable : 4244)
#include <DirectXMath.h>
#include "Easing.h"

//------------------------------------------------------------------------------
//
//  イージング関数
//
//------------------------------------------------------------------------------

// Linear
float Linear::ease(float t, float b, float c, float d)
{
    return b * (1.0f - (t / d)) + (c * (t / d));
}

// Sine
float Sine::easeIn(float t, float b, float c, float d) {
    return -c * cosf(t / d * (DirectX::XM_PI / 2)) + c + b;
}

float Sine::easeOut(float t, float b, float c, float d) {
    return c * sinf(t / d * (DirectX::XM_PI / 2)) + b;
}

float Sine::easeInOut(float t, float b, float c, float d) {
    return -c / 2 * (cosf(DirectX::XM_PI * t / d) - 1) + b;
}

// Quad
float Quad::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t + b;
}
float Quad::easeOut(float t, float b, float c, float d) {
    return -c * (t /= d) * (t - 2) + b;
}

float Quad::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
    return -c / 2 * (((t - 2) * (--t)) - 1) + b;
}

// Cubic
float Cubic::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t + b;
}
float Cubic::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t + 1) + b;
}

float Cubic::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t + 2) + b;
}

// Quart
float Quart::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t + b;
}
float Quart::easeOut(float t, float b, float c, float d) {
    return -c * ((t = t / d - 1) * t * t * t - 1) + b;
}

float Quart::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t + b;
    return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
}

// Quint
float Quint::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t * t + b;
}
float Quint::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
}

float Quint::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
}

// Expo
float Expo::easeIn(float t, float b, float c, float d) {
    return (t == 0) ? b : c * pow(2, 10 * (t / d - 1)) + b;
}
float Expo::easeOut(float t, float b, float c, float d) {
    return (t == d) ? b + c : c * (-pow(2, -10 * t / d) + 1) + b;
}

float Expo::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;
    if (t == d) return b + c;
    if ((t /= d / 2) < 1) return c / 2 * pow(2, 10 * (t - 1)) + b;
    return c / 2 * (-pow(2, -10 * --t) + 2) + b;
}

// Circ
float Circ::easeIn(float t, float b, float c, float d) {
    return -c * (sqrt(1 - (t /= d) * t) - 1) + b;
}
float Circ::easeOut(float t, float b, float c, float d) {
    return c * sqrt(1 - (t = t / d - 1) * t) + b;
}

float Circ::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return -c / 2 * (sqrt(1 - t * t) - 1) + b;
    return c / 2 * (sqrt(1 - t * (t -= 2)) + 1) + b;
}

// Back
float Back::easeIn(float t, float b, float c, float d) {
    float s = 1.70158f;
    float postFix = t /= d;
    return c * (postFix)*t * ((s + 1) * t - s) + b;
}
float Back::easeOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
}

float Back::easeInOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    if ((t /= d / 2) < 1) return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
    float postFix = t -= 2;
    return c / 2 * ((postFix)*t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}

// Elastic
float Elastic::easeIn(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    float postFix = a * pow(2, 10 * (t -= 1));
    return -(postFix * sin((t * d - s) * (2 * DirectX::XM_PI) / p)) + b;
}

float Elastic::easeOut(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    return (a * pow(2, -10 * t) * sin((t * d - s) * (2 * DirectX::XM_PI) / p) + c + b);
}

float Elastic::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
    float p = d * (.3f * 1.5f);
    float a = c;
    float s = p / 4;

    if (t < 1) {
        float postFix = a * pow(2, 10 * (t -= 1));
        return -.5f * (postFix * sin((t * d - s) * (2 * DirectX::XM_PI) / p)) + b;
    }
    float postFix = a * pow(2, -10 * (t -= 1));
    return postFix * sin((t * d - s) * (2 * DirectX::XM_PI) / p) * .5f + c + b;
}

// Bounce
float Bounce::easeIn(float t, float b, float c, float d) {
    return c - easeOut(d - t, 0, c, d) + b;
}
float Bounce::easeOut(float t, float b, float c, float d) {
    if ((t /= d) < (1 / 2.75f)) {
        return c * (7.5625f * t * t) + b;
    }
    else if (t < (2 / 2.75f)) {
        float postFix = t -= (1.5f / 2.75f);
        return c * (7.5625f * (postFix)*t + .75f) + b;
    }
    else if (t < (2.5 / 2.75)) {
        float postFix = t -= (2.25f / 2.75f);
        return c * (7.5625f * (postFix)*t + .9375f) + b;
    }
    else {
        float postFix = t -= (2.625f / 2.75f);
        return c * (7.5625f * (postFix)*t + .984375f) + b;
    }
}

float Bounce::easeInOut(float t, float b, float c, float d) {
    if (t < d / 2) return easeIn(t * 2, 0, c, d) * .5f + b;
    else return easeOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}

float Easing::Other(EaseInOutType easeInOutType, EaseType easeType, float t, float b, float c, float d)
{
    switch (easeInOutType)
    {
    case EaseInOutType::In:
        return In(easeType, t, b, c, d);
        break;
    case EaseInOutType::Out:
        return Out(easeType, t, b, c, d);
        break;
    case EaseInOutType::InOut:
        return InOut(easeType, t, b, c, d);
        break;
    }
    return 0.0f;
}

float Easing::In(EaseType easeType, float t, float b, float c, float d)
{
    switch (easeType)
    {
    case EaseType::Linear:
        return Linear::ease(t, b, c, d);
        break;
    case EaseType::Sine:
        return Sine::easeIn(t, b, c, d);
        break;
    case EaseType::Quad:
        return Quad::easeIn(t, b, c, d);
        break;
    case EaseType::Cubic:
        return Cubic::easeIn(t, b, c, d);
        break;
    case EaseType::Quart:
        return Quart::easeIn(t, b, c, d);
        break;
    case EaseType::Quint:
        return Quint::easeIn(t, b, c, d);
        break;
    case EaseType::Expo:
        return Expo::easeIn(t, b, c, d);
        break;
    case EaseType::Circ:
        return Circ::easeIn(t, b, c, d);
        break;
    case EaseType::Back:
        return Back::easeIn(t, b, c, d);
        break;
    case EaseType::Elastic:
        return Elastic::easeIn(t, b, c, d);
        break;
    case EaseType::Bounce:
        return Bounce::easeIn(t, b, c, d);
        break;
    }
    return 0.0f;
}

float Easing::Out(EaseType easeType, float t, float b, float c, float d)
{
    switch (easeType)
    {
    case EaseType::Linear:
        return Linear::ease(t, b, c, d);
        break;
    case EaseType::Sine:
        return Sine::easeOut(t, b, c, d);
        break;
    case EaseType::Quad:
        return Quad::easeOut(t, b, c, d);
        break;
    case EaseType::Cubic:
        return Cubic::easeOut(t, b, c, d);
        break;
    case EaseType::Quart:
        return Quart::easeOut(t, b, c, d);
        break;
    case EaseType::Quint:
        return Quint::easeOut(t, b, c, d);
        break;
    case EaseType::Expo:
        return Expo::easeOut(t, b, c, d);
        break;
    case EaseType::Circ:
        return Circ::easeOut(t, b, c, d);
        break;
    case EaseType::Back:
        return Back::easeOut(t, b, c, d);
        break;
    case EaseType::Elastic:
        return Elastic::easeOut(t, b, c, d);
        break;
    case EaseType::Bounce:
        return Bounce::easeOut(t, b, c, d);
        break;
    }
    return 0.0f;
}

float Easing::InOut(EaseType easeType, float t, float b, float c, float d)
{
    switch (easeType)
    {
    case EaseType::Linear:
        return Linear::ease(t, b, c, d);
        break;
    case EaseType::Sine:
        return Sine::easeInOut(t, b, c, d);
        break;
    case EaseType::Quad:
        return Quad::easeInOut(t, b, c, d);
        break;
    case EaseType::Cubic:
        return Cubic::easeInOut(t, b, c, d);
        break;
    case EaseType::Quart:
        return Quart::easeInOut(t, b, c, d);
        break;
    case EaseType::Quint:
        return Quint::easeInOut(t, b, c, d);
        break;
    case EaseType::Expo:
        return Expo::easeInOut(t, b, c, d);
        break;
    case EaseType::Circ:
        return Circ::easeInOut(t, b, c, d);
        break;
    case EaseType::Back:
        return Back::easeInOut(t, b, c, d);
        break;
    case EaseType::Elastic:
        return Elastic::easeInOut(t, b, c, d);
        break;
    case EaseType::Bounce:
        return Bounce::easeInOut(t, b, c, d);
        break;
    }
    return 0.0f;
}

// イージングの方向の文字列表現
const char* EaseInOutTypeToString(EaseInOutType type)
{
    switch (type)
    {
    case EaseInOutType::In: return "In";
    case EaseInOutType::Out: return "Out";
    case EaseInOutType::InOut: return "InOut";
    default: return "Unknown";
    }
}

// イージングのタイプの文字列表現
const char* EaseTypeToString(EaseType type)
{
    switch (type)
    {
    case EaseType::Linear: return "Linear";
    case EaseType::Sine: return "Sine";
    case EaseType::Quad: return "Quad";
    case EaseType::Cubic: return "Cubic";
    case EaseType::Quart: return "Quart";
    case EaseType::Quint: return "Quint";
    case EaseType::Expo: return "Expo";
    case EaseType::Circ: return "Circ";
    case EaseType::Back: return "Back";
    case EaseType::Elastic: return "Elastic";
    case EaseType::Bounce: return "Bounce";
    default: return "Unknown";
    }
}

//imgui
float EasingImGui(int& easingtype, int& easingmovetype, float& t)
{
    EaseType selectedEaseType = static_cast<EaseType>(easingtype); // 初期選択値
    EaseInOutType selectedEaseInOutType = static_cast<EaseInOutType>(easingmovetype); // 初期選択値

    // イージングタイプのドロップダウンメニューを作成
    const char* easeTypeItems[(int)EaseType::Max];
    for (int i = 0; i < (int)EaseType::Max; ++i)
    {
        easeTypeItems[i] = EaseTypeToString(static_cast<EaseType>(i));
    }

    if (ImGui::Combo("Easing Type", &easingtype, easeTypeItems, (int)EaseType::Max))
    {
        selectedEaseType = static_cast<EaseType>(easingtype);
    }

    // イージング方向のドロップダウンメニューを作成
    const char* easeInOutTypeItems[(int)EaseInOutType::Max];
    for (int i = 0; i < (int)EaseInOutType::Max; ++i)
    {
        easeInOutTypeItems[i] = EaseInOutTypeToString(static_cast<EaseInOutType>(i));
    }

    if (ImGui::Combo("Ease In/Out Type", &easingmovetype, easeInOutTypeItems, (int)EaseInOutType::Max))
    {
        selectedEaseInOutType = static_cast<EaseInOutType>(easingmovetype);
    }

    // tのドラッグ設定
    ImGui::DragFloat("Time (t)", &t, 0.001f, 0.0f, 1.0f, "%.3f");

    // 選択されたイージングタイプと方向に基づいてイージング関数を呼び出す
    float result = 0.0f;

    switch (selectedEaseInOutType)
    {
    case EaseInOutType::In:
        result = Easing::In(selectedEaseType, t);
        break;
    case EaseInOutType::Out:
        result = Easing::Out(selectedEaseType, t);
        break;
    case EaseInOutType::InOut:
        result = Easing::InOut(selectedEaseType, t);
        break;
    }

    ImGui::Text("Result: %f", result);

    return result;
}