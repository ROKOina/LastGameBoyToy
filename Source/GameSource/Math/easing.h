#pragma once

//******************************************************************************
//
//		Easing.h
//
//******************************************************************************

#include <math.h>
#include <stdio.h>
#include "imgui.h"

enum class EaseType
{
    Linear,
    Sine,
    Quad,
    Cubic,
    Quart,
    Quint,
    Expo,
    Circ,
    Back,
    Elastic,
    Bounce,
    Max,
};

enum class EaseInOutType
{
    In,
    Out,
    InOut,
    Max,
};

class Easing
{
public:
    static float Other(EaseInOutType easeInOutType, EaseType easeType, float time, float startValue = 0, float endValue = 1, float endTime = 1);
    static float In(EaseType easeType, float time, float startValue = 0, float endValue = 1, float endTime = 1);
    static float Out(EaseType easeType, float time, float startValue = 0, float endValue = 1, float endTime = 1);
    static float InOut(EaseType easeType, float time, float startValue = 0, float endValue = 1, float endTime = 1);
};

//イージング関数
class Linear {
public:

    static float ease(float t, float b = 0, float c = 1, float d = 1);
};

class Sine {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Quad {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Cubic {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Quart {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Quint {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Expo {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Circ {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Back {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Elastic {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

class Bounce {
public:

    static float easeIn(float t, float b = 0, float c = 1, float d = 1);
    static float easeOut(float t, float b = 0, float c = 1, float d = 1);
    static float easeInOut(float t, float b = 0, float c = 1, float d = 1);
};

//imgui表示
float EasingImGui(int& easingtype, int& easingmovetype, float& t);