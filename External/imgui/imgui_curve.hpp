// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/55

// v1.22 - flip button; cosmetic fixes
// v1.21 - oops :)
// v1.20 - add iq's interpolation code
// v1.10 - easing and colors        
// v1.00 - jari komppa's original

#pragma once

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <cmath>

/* To use, add this prototype somewhere..

namespace ImGui
{
    int Curve(const char *label, const ImVec2& size, int maxpoints, ImVec2 *points);
    float CurveValue(float p, int maxpoints, const ImVec2 *points);
    float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points);
};

*/
/*
    Example of use:

    ImVec2 foo[10];
    ...
    foo[0].x = -1; // init data so editor knows to take it from here
    ...
    if (ImGui::Curve("Das editor", ImVec2(600, 200), 10, foo))
    {
        // curve changed
    }
    ...
    float value_you_care_about = ImGui::CurveValue(0.7f, 10, foo); // calculate value at position 0.7
*/

static inline float ImRemap(float v, float a, float b, float c, float d)
{
    return (c + (d - c) * (v - a) / (b - a));
}

static inline ImVec2 ImRemap(const ImVec2& v, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d)
{
    return ImVec2(ImRemap(v.x, a.x, b.x, c.x, d.x), ImRemap(v.y, a.y, b.y, c.y, d.y));
}

namespace tween {
    enum TYPE
    {
        LINEAR,

        QUADIN,          // t^2
        QUADOUT,
        QUADINOUT,
        CUBICIN,         // t^3
        CUBICOUT,
        CUBICINOUT,
        QUARTIN,         // t^4
        QUARTOUT,
        QUARTINOUT,
        QUINTIN,         // t^5
        QUINTOUT,
        QUINTINOUT,
        SINEIN,          // sin(t)
        SINEOUT,
        SINEINOUT,
        EXPOIN,          // 2^t
        EXPOOUT,
        EXPOINOUT,
        CIRCIN,          // sqrt(1-t^2)
        CIRCOUT,
        CIRCINOUT,
        ELASTICIN,       // exponentially decaying sine wave
        ELASTICOUT,
        ELASTICINOUT,
        BACKIN,          // overshooting cubic easing: (s+1)*t^3 - s*t^2
        BACKOUT,
        BACKINOUT,
        BOUNCEIN,        // exponentially decaying parabolic bounce
        BOUNCEOUT,
        BOUNCEINOUT,

        SINESQUARE,      // gapjumper's
        EXPONENTIAL,     // gapjumper's
        SCHUBRING1,      // terry schubring's formula 1
        SCHUBRING2,      // terry schubring's formula 2
        SCHUBRING3,      // terry schubring's formula 3

        SINPI2,          // tomas cepeda's
        SWING,           // tomas cepeda's & lquery's
    };

    // }

    // implementation

    static inline
        double ease(int easetype, double t)
    {
        using namespace std;

        const double d = 1.f;
        const double pi = 3.1415926535897932384626433832795;
        const double pi2 = 3.1415926535897932384626433832795 / 2;

        double p = t / d;

        switch (easetype)
        {
            // Modeled after the line y = x
        default:
        case TYPE::LINEAR: {
            return p;
        }

                         // Modeled after the parabola y = x^2
        case TYPE::QUADIN: {
            return p * p;
        }

                         // Modeled after the parabola y = -x^2 + 2x
        case TYPE::QUADOUT: {
            return -(p * (p - 2));
        }

                          // Modeled after the piecewise quadratic
                          // y = (1/2)((2x)^2)             ; [0, 0.5)
                          // y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
        case TYPE::QUADINOUT: {
            if (p < 0.5) {
                return 2 * p * p;
            }
            else {
                return (-2 * p * p) + (4 * p) - 1;
            }
        }

                            // Modeled after the cubic y = x^3
        case TYPE::CUBICIN: {
            return p * p * p;
        }

                          // Modeled after the cubic y = (x - 1)^3 + 1
        case TYPE::CUBICOUT: {
            double f = (p - 1);
            return f * f * f + 1;
        }

                           // Modeled after the piecewise cubic
                           // y = (1/2)((2x)^3)       ; [0, 0.5)
                           // y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
        case TYPE::CUBICINOUT: {
            if (p < 0.5) {
                return 4 * p * p * p;
            }
            else {
                double f = ((2 * p) - 2);
                return 0.5 * f * f * f + 1;
            }
        }

                             // Modeled after the quartic x^4
        case TYPE::QUARTIN: {
            return p * p * p * p;
        }

                          // Modeled after the quartic y = 1 - (x - 1)^4
        case TYPE::QUARTOUT: {
            double f = (p - 1);
            return f * f * f * (1 - p) + 1;
        }

                           // Modeled after the piecewise quartic
                           // y = (1/2)((2x)^4)        ; [0, 0.5)
                           // y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
        case TYPE::QUARTINOUT: {
            if (p < 0.5) {
                return 8 * p * p * p * p;
            }
            else {
                double f = (p - 1);
                return -8 * f * f * f * f + 1;
            }
        }

                             // Modeled after the quintic y = x^5
        case TYPE::QUINTIN: {
            return p * p * p * p * p;
        }

                          // Modeled after the quintic y = (x - 1)^5 + 1
        case TYPE::QUINTOUT: {
            double f = (p - 1);
            return f * f * f * f * f + 1;
        }

                           // Modeled after the piecewise quintic
                           // y = (1/2)((2x)^5)       ; [0, 0.5)
                           // y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
        case TYPE::QUINTINOUT: {
            if (p < 0.5) {
                return 16 * p * p * p * p * p;
            }
            else {
                double f = ((2 * p) - 2);
                return  0.5 * f * f * f * f * f + 1;
            }
        }

                             // Modeled after quarter-cycle of sine wave
        case TYPE::SINEIN: {
            return sin((p - 1) * pi2) + 1;
        }

                         // Modeled after quarter-cycle of sine wave (different phase)
        case TYPE::SINEOUT: {
            return sin(p * pi2);
        }

                          // Modeled after half sine wave
        case TYPE::SINEINOUT: {
            return 0.5 * (1 - cos(p * pi));
        }

                            // Modeled after shifted quadrant IV of unit circle
        case TYPE::CIRCIN: {
            return 1 - sqrt(1 - (p * p));
        }

                         // Modeled after shifted quadrant II of unit circle
        case TYPE::CIRCOUT: {
            return sqrt((2 - p) * p);
        }

                          // Modeled after the piecewise circular function
                          // y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
                          // y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
        case TYPE::CIRCINOUT: {
            if (p < 0.5) {
                return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
            }
            else {
                return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
            }
        }

                            // Modeled after the exponential function y = 2^(10(x - 1))
        case TYPE::EXPOIN: {
            return (p == 0.0) ? p : pow(2, 10 * (p - 1));
        }

                         // Modeled after the exponential function y = -2^(-10x) + 1
        case TYPE::EXPOOUT: {
            return (p == 1.0) ? p : 1 - pow(2, -10 * p);
        }

                          // Modeled after the piecewise exponential
                          // y = (1/2)2^(10(2x - 1))         ; [0,0.5)
                          // y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
        case TYPE::EXPOINOUT: {
            if (p == 0.0 || p == 1.0) return p;

            if (p < 0.5) {
                return 0.5 * pow(2, (20 * p) - 10);
            }
            else {
                return -0.5 * pow(2, (-20 * p) + 10) + 1;
            }
        }

                            // Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
        case TYPE::ELASTICIN: {
            return sin(13 * pi2 * p) * pow(2, 10 * (p - 1));
        }

                            // Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
        case TYPE::ELASTICOUT: {
            return sin(-13 * pi2 * (p + 1)) * pow(2, -10 * p) + 1;
        }

                             // Modeled after the piecewise exponentially-damped sine wave:
                             // y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
                             // y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
        case TYPE::ELASTICINOUT: {
            if (p < 0.5) {
                return 0.5 * sin(13 * pi2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
            }
            else {
                return 0.5 * (sin(-13 * pi2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
            }
        }

                               // Modeled (originally) after the overshooting cubic y = x^3-x*sin(x*pi)
        case TYPE::BACKIN: { /*
            return p * p * p - p * sin(p * pi); */
            double s = 1.70158f;
            return p * p * ((s + 1) * p - s);
        }

                         // Modeled (originally) after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
        case TYPE::BACKOUT: { /*
            double f = (1 - p);
            return 1 - (f * f * f - f * sin(f * pi)); */
            double s = 1.70158f;
            return --p, 1.f * (p * p * ((s + 1) * p + s) + 1);
        }

                          // Modeled (originally) after the piecewise overshooting cubic function:
                          // y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
                          // y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
        case TYPE::BACKINOUT: { /*
            if(p < 0.5) {
                double f = 2 * p;
                return 0.5 * (f * f * f - f * sin(f * pi));
            }
            else {
                double f = (1 - (2*p - 1));
                return 0.5 * (1 - (f * f * f - f * sin(f * pi))) + 0.5;
            } */
            double s = 1.70158f * 1.525f;
            if (p < 0.5) {
                return p *= 2, 0.5 * p * p * (p * s + p - s);
            }
            else {
                return p = p * 2 - 2, 0.5 * (2 + p * p * (p * s + p + s));
            }
        }

#           define tween$bounceout(p) ( \
                (p) < 4/11.0 ? (121 * (p) * (p))/16.0 : \
                (p) < 8/11.0 ? (363/40.0 * (p) * (p)) - (99/10.0 * (p)) + 17/5.0 : \
                (p) < 9/10.0 ? (4356/361.0 * (p) * (p)) - (35442/1805.0 * (p)) + 16061/1805.0 \
                           : (54/5.0 * (p) * (p)) - (513/25.0 * (p)) + 268/25.0 )

        case TYPE::BOUNCEIN: {
            return 1 - tween$bounceout(1 - p);
        }

        case TYPE::BOUNCEOUT: {
            return tween$bounceout(p);
        }

        case TYPE::BOUNCEINOUT: {
            if (p < 0.5) {
                return 0.5 * (1 - tween$bounceout(1 - p * 2));
            }
            else {
                return 0.5 * tween$bounceout((p * 2 - 1)) + 0.5;
            }
        }

#           undef tween$bounceout

        case TYPE::SINESQUARE: {
            double A = sin((p)*pi2);
            return A * A;
        }

        case TYPE::EXPONENTIAL: {
            return 1 / (1 + exp(6 - 12 * (p)));
        }

        case TYPE::SCHUBRING1: {
            return 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
        }

        case TYPE::SCHUBRING2: {
            double p1pass = 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
            double p2pass = 2 * (p1pass + (0.5f - p1pass) * abs(0.5f - p1pass)) - 0.5f;
            double pAvg = (p1pass + p2pass) / 2;
            return pAvg;
        }

        case TYPE::SCHUBRING3: {
            double p1pass = 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
            double p2pass = 2 * (p1pass + (0.5f - p1pass) * abs(0.5f - p1pass)) - 0.5f;
            return p2pass;
        }

        case TYPE::SWING: {
            return ((-cos(pi * p) * 0.5) + 0.5);
        }

        case TYPE::SINPI2: {
            return sin(p * pi2);
        }
        }
    }
}

static constexpr float HANDLE_RADIUS = 4;

namespace ImGui
{
    enum class CurveEditorFlags
    {
        NO_TANGENTS = 1 << 0,
        SHOW_GRID = 1 << 1,
        RESET = 1 << 2
    };

    enum class PinShape {
        CIRCLE,
        TRIANGLE,
        SQUARE
    };

    // [src] http://iquilezles.org/www/articles/minispline/minispline.htm
    // key format (for dim == 1) is (t0,x0,t1,x1 ...)
    // key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
    // key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
    void spline(const float* key, int num, int dim, float t, float* v)
    {
        static signed char coefs[16] = {
            -1, 2,-1, 0,
             3,-5, 0, 2,
            -3, 4, 1, 0,
             1,-1, 0, 0 };

        const int size = dim + 1;

        // find key
        int k = 0; while (key[k * size] < t) k++;

        // interpolant
        const float h = (t - key[(k - 1) * size]) / (key[k * size] - key[(k - 1) * size]);

        // init result
        for (int i = 0; i < dim; i++) v[i] = 0.0f;

        // add basis functions
        for (int i = 0; i < 4; i++)
        {
            int kn = k + i - 2; if (kn < 0) kn = 0; else if (kn > (num - 1)) kn = num - 1;

            const signed char* co = coefs + 4 * i;

            const float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

            for (int j = 0; j < dim; j++) v[j] += b * key[kn * size + j + 1];
        }
    }

    float CurveValueSmooth(float p, int maxpoints, const ImVec2* points)
    {
        if (maxpoints < 2 || points == 0)
            return 0;
        if (p < 0) return points[0].y;

        float* input = new float[maxpoints * 2];
        float output[4];

        for (int i = 0; i < maxpoints; ++i) {
            input[i * 2 + 0] = points[i].x;
            input[i * 2 + 1] = points[i].y;
        }

        spline(input, maxpoints, 1, p, output);

        delete[] input;
        return output[0];
    }

    float CurveValue(float p, int maxpoints, const ImVec2* points)
    {
        if (maxpoints < 2 || points == 0)
            return 0;
        if (p < 0) return points[0].y;

        int left = 0;
        while (left < maxpoints && points[left].x < p && points[left].x != -1) left++;
        if (left) left--;

        if (left == maxpoints - 1)
            return points[maxpoints - 1].y;

        float d = (p - points[left].x) / (points[left + 1].x - points[left].x);

        return points[left].y + (points[left + 1].y - points[left].y) * d;
    }

    int Curve(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points)
    {
        int modified = 0;
        int i;
        if (maxpoints < 2 || points == 0)
            return 0;

        if (points[0].x < 0)
        {
            points[0].x = 0;
            points[0].y = 0;
            points[1].x = 1;
            points[1].y = 1;
            points[2].x = -1;
        }

        ImGuiWindow* window = GetCurrentWindow();
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        if (window->SkipItems)
            return 0;

        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
        ItemSize(bb);
        if (!ItemAdd(bb, NULL))
            return 0;

        const bool hovered = ImGui::ItemHoverable(bb, id);

        int max = 0;
        while (max < maxpoints && points[max].x >= 0) max++;

        int kill = 0;
        do
        {
            if (kill)
            {
                modified = 1;
                for (i = kill + 1; i < max; i++)
                {
                    points[i - 1] = points[i];
                }
                max--;
                points[max].x = -1;
                kill = 0;
            }

            for (i = 1; i < max - 1; i++)
            {
                if (abs(points[i].x - points[i - 1].x) < 1 / 128.0)
                {
                    kill = i;
                }
            }
        } while (kill);


        RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

        float ht = bb.Max.y - bb.Min.y;
        float wd = bb.Max.x - bb.Min.x;

        if (hovered)
        {
            SetHoveredID(id);
            if (g.IO.MouseDown[0])
            {
                modified = 1;
                ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
                pos.y = 1 - pos.y;

                int left = 0;
                while (left < max && points[left].x < pos.x) left++;
                if (left) left--;

                ImVec2 p = points[left] - pos;
                float p1d = sqrt(p.x * p.x + p.y * p.y);
                p = points[left + 1] - pos;
                float p2d = sqrt(p.x * p.x + p.y * p.y);
                int sel = -1;
                if (p1d < (1 / 16.0)) sel = left;
                if (p2d < (1 / 16.0)) sel = left + 1;

                if (sel != -1)
                {
                    points[sel] = pos;
                }
                else
                {
                    if (max < maxpoints)
                    {
                        max++;
                        for (i = max; i > left; i--)
                        {
                            points[i] = points[i - 1];
                        }
                        points[left + 1] = pos;
                    }
                    if (max < maxpoints)
                        points[max].x = -1;
                }

                // snap first/last to min/max
                if (points[0].x < points[max - 1].x) {
                    points[0].x = 0;
                    points[max - 1].x = 1;
                }
                else {
                    points[0].x = 1;
                    points[max - 1].x = 0;
                }
            }
        }

        // bg grid
        window->DrawList->AddLine(
            ImVec2(bb.Min.x, bb.Min.y + ht / 2),
            ImVec2(bb.Max.x, bb.Min.y + ht / 2),
            GetColorU32(ImGuiCol_TextDisabled), 3);

        window->DrawList->AddLine(
            ImVec2(bb.Min.x, bb.Min.y + ht / 4),
            ImVec2(bb.Max.x, bb.Min.y + ht / 4),
            GetColorU32(ImGuiCol_TextDisabled));

        window->DrawList->AddLine(
            ImVec2(bb.Min.x, bb.Min.y + ht / 4 * 3),
            ImVec2(bb.Max.x, bb.Min.y + ht / 4 * 3),
            GetColorU32(ImGuiCol_TextDisabled));

        for (i = 0; i < 9; i++)
        {
            window->DrawList->AddLine(
                ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Min.y),
                ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Max.y),
                GetColorU32(ImGuiCol_TextDisabled));
        }

        // smooth curve
        enum { smoothness = 256 }; // the higher the smoother
        for (i = 0; i <= (smoothness - 1); ++i) {
            float px = (i + 0) / float(smoothness);
            float qx = (i + 1) / float(smoothness);
            float py = 1 - CurveValueSmooth(px, maxpoints, points);
            float qy = 1 - CurveValueSmooth(qx, maxpoints, points);
            ImVec2 p(px * (bb.Max.x - bb.Min.x) + bb.Min.x, py * (bb.Max.y - bb.Min.y) + bb.Min.y);
            ImVec2 q(qx * (bb.Max.x - bb.Min.x) + bb.Min.x, qy * (bb.Max.y - bb.Min.y) + bb.Min.y);
            window->DrawList->AddLine(p, q, GetColorU32(ImGuiCol_PlotLines));
        }

        // lines
        for (i = 1; i < max; i++)
        {
            ImVec2 a = points[i - 1];
            ImVec2 b = points[i];
            a.y = 1 - a.y;
            b.y = 1 - b.y;
            a = a * (bb.Max - bb.Min) + bb.Min;
            b = b * (bb.Max - bb.Min) + bb.Min;
            window->DrawList->AddLine(a, b, GetColorU32(ImGuiCol_PlotLinesHovered));
        }

        if (hovered)
        {
            // control points
            for (i = 0; i < max; i++)
            {
                ImVec2 p = points[i];
                p.y = 1 - p.y;
                p = p * (bb.Max - bb.Min) + bb.Min;
                ImVec2 a = p - ImVec2(2, 2);
                ImVec2 b = p + ImVec2(2, 2);
                window->DrawList->AddRect(a, b, GetColorU32(ImGuiCol_PlotLinesHovered));
            }
        }

        // buttons; @todo: mirror, smooth, tessellate
        if (ImGui::Button("Flip")) {
            for (i = 0; i < max; ++i) {
                points[i].y = 1 - points[i].y;
            }
        }
        ImGui::SameLine();

        // curve selector
        const char* items[] = {
            "Custom",

            "Linear",
            "Quad in",
            "Quad out",
            "Quad in  out",
            "Cubic in",
            "Cubic out",
            "Cubic in  out",
            "Quart in",
            "Quart out",
            "Quart in  out",
            "Quint in",
            "Quint out",
            "Quint in  out",
            "Sine in",
            "Sine out",
            "Sine in  out",
            "Expo in",
            "Expo out",
            "Expo in  out",
            "Circ in",
            "Circ out",
            "Circ in  out",
            "Elastic in",
            "Elastic out",
            "Elastic in  out",
            "Back in",
            "Back out",
            "Back in  out",
            "Bounce in",
            "Bounce out",
            "Bounce in out",

            "Sine square",
            "Exponential",

            "Schubring1",
            "Schubring2",
            "Schubring3",

            "SinPi2",
            "Swing"
        };
        static int item = 0;
        if (modified) {
            item = 0;
        }
        if (ImGui::Combo("Ease type", &item, items, IM_ARRAYSIZE(items))) {
            max = maxpoints;
            if (item > 0) {
                for (i = 0; i < max; ++i) {
                    points[i].x = i / float(max - 1);
                    points[i].y = float(tween::ease(item - 1, points[i].x));
                }
            }
        }

        char buf[128];
        const char* str = label;

        if (hovered) {
            ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
            pos.y = 1 - pos.y;

            snprintf(buf, sizeof(buf), "%s (%.2f,%.2f)", label, pos.x, pos.y);
            str = buf;
        }

        RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), bb.Max, str, NULL, NULL, ImVec2(0.5f, 0.5f));

        return modified;
    }


    int CurveEditor(const char* label
        , float* values
        , int points_count
        , int capacity
        , const ImVec2& editor_size = ImVec2(-1, -1)
        , ImU32 flags = 0
        , int* new_count = nullptr
        , int* selected_point = nullptr
        , float curvePoint = 0.0f
        , bool* isSelectable = nullptr
        , bool* isHovered = nullptr
        , int* hovered_point = nullptr)
    {
        enum class StorageValues : ImGuiID
        {
            FROM_X = 100,
            FROM_Y,
            WIDTH,
            HEIGHT,
            IS_PANNING,
            POINT_START_X,
            POINT_START_Y
        };

        const float HEIGHT = 100;
        static ImVec2 start_pan;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        ImVec2 size = editor_size;
        size.x = size.x < 0 ? CalcItemWidth() + (style.FramePadding.x * 2) : size.x;
        size.y = size.y < 0 ? HEIGHT : size.y;
        if (hovered_point) *hovered_point = -1;

        ImGuiWindow* parent_window = GetCurrentWindow();
        ImGuiID id = parent_window->GetID(label);
        if (new_count) *new_count = points_count;
        if (!BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            EndChildFrame();
            return -1;
        }

        int hovered_idx = -1;

        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems)
        {
            EndChildFrame();
            return -1;
        }

        ImVec2 points_min(FLT_MAX, FLT_MAX);
        ImVec2 points_max(-FLT_MAX, -FLT_MAX);
        for (int point_idx = 0; point_idx < points_count; ++point_idx)
        {
            ImVec2 point;
            if (flags & (int)CurveEditorFlags::NO_TANGENTS)
            {
                point = ((ImVec2*)values)[point_idx];
            }
            else
            {
                point = ((ImVec2*)values)[1 + point_idx * 3];
            }
            points_max = ImMax(points_max, point);
            points_min = ImMin(points_min, point);
        }
        points_max.y = ImMax(points_max.y, points_min.y + 0.0001f);

        ImVec2 points_range = points_max - points_min;
        points_min -= points_range * 0.05f;
        points_max += points_range * 0.05f;

        if (flags & (int)CurveEditorFlags::RESET) window->StateStorage.Clear();

        float from_x = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_X, points_min.x);
        float from_y = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_Y, points_min.y);
        float width = window->StateStorage.GetFloat((ImGuiID)StorageValues::WIDTH, points_max.x - points_min.x);
        float height = window->StateStorage.GetFloat((ImGuiID)StorageValues::HEIGHT, points_max.y - points_min.y);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);

        const ImRect inner_bb = window->InnerClipRect;
        if (inner_bb.GetWidth() == 0 || inner_bb.GetHeight() == 0) {
            EndChildFrame();
            return -1;
        }
        const ImRect frame_bb(inner_bb.Min - style.FramePadding, inner_bb.Max + style.FramePadding);

        auto transform = [&](const ImVec2& pos) -> ImVec2
            {
                float x = (pos.x - from_x) / width;
                float y = (pos.y - from_y) / height;

                return ImVec2(
                    inner_bb.Min.x * (1 - x) + inner_bb.Max.x * x,
                    inner_bb.Min.y * y + inner_bb.Max.y * (1 - y)
                );
            };

        auto invTransform = [&](const ImVec2& pos) -> ImVec2
            {
                float x = (pos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x);
                float y = (inner_bb.Max.y - pos.y) / (inner_bb.Max.y - inner_bb.Min.y);

                return ImVec2(
                    from_x + width * x,
                    from_y + height * y
                );
            };

        const ImU32 color_text = GetColorU32(ImGuiCol_Text);
        if (flags & (int)CurveEditorFlags::SHOW_GRID)
        {
            int exp;
            frexp(width / 5, &exp);
            float step_x = (float)ldexp(1.0, exp);
            int cell_cols = int(width / step_x);

            float x = step_x * int(from_x / step_x);
            for (int i = -1; i < cell_cols + 2; ++i)
            {
                ImVec2 a = transform({ x + i * step_x, from_y });
                ImVec2 b = transform({ x + i * step_x, from_y + height });
                window->DrawList->AddLine(a, b, 0x55000000);
                char buf[64];
                if (exp > 0)
                {
                    ImFormatString(buf, sizeof(buf), " %d", int(x + i * step_x));
                }
                else
                {
                    ImFormatString(buf, sizeof(buf), " %.2f", x + i * step_x);
                }
                window->DrawList->AddText(b, color_text, buf);
            }

            frexp(height / 5, &exp);
            float step_y = (float)ldexp(1.0, exp);
            int cell_rows = int(height / step_y);

            float y = step_y * int(from_y / step_y);
            for (int i = -1; i < cell_rows + 2; ++i)
            {
                ImVec2 a = transform({ from_x, y + i * step_y });
                ImVec2 b = transform({ from_x + width, y + i * step_y });
                window->DrawList->AddLine(a, b, 0x55000000);
                char buf[64];
                if (exp > 0)
                {
                    ImFormatString(buf, sizeof(buf), " %d", int(y + i * step_y));
                }
                else
                {
                    ImFormatString(buf, sizeof(buf), " %.2f", y + i * step_y);
                }
                window->DrawList->AddText(a, color_text, buf);
            }
        }

        const ImGuiID dragger_id = GetID("##_node_dragger");
        ImGui::ItemAdd(inner_bb, dragger_id);

        if (GetIO().MouseWheel != 0 && IsItemHovered())
        {
            float scale = powf(2, -GetIO().MouseWheel);
            width *= scale;
            height *= scale;
            window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
            window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
        }
        if (IsMouseReleased(2))
        {
            window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, false);
        }
        if (window->StateStorage.GetBool((ImGuiID)StorageValues::IS_PANNING, false))
        {
            ImVec2 drag_offset = GetMouseDragDelta(2);
            from_x = start_pan.x;
            from_y = start_pan.y;
            from_x -= drag_offset.x * width / (inner_bb.Max.x - inner_bb.Min.x);
            from_y += drag_offset.y * height / (inner_bb.Max.y - inner_bb.Min.y);
            window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
            window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
        }
        else if (IsMouseDragging(2) && IsItemHovered())
        {
            window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, true);
            start_pan.x = from_x;
            start_pan.y = from_y;
        }

        int changed_idx = -1;
        for (int point_idx = points_count - 2; point_idx >= 0; --point_idx)
        {
            ImVec2* points;
            if (flags & (int)CurveEditorFlags::NO_TANGENTS)
            {
                points = ((ImVec2*)values) + point_idx;
            }
            else
            {
                points = ((ImVec2*)values) + 1 + point_idx * 3;
            }

            ImVec2 p_prev = points[0];
            ImVec2 tangent_last;
            ImVec2 tangent;
            ImVec2 p;
            if (flags & (int)CurveEditorFlags::NO_TANGENTS)
            {
                p = points[1];
            }
            else
            {
                tangent_last = points[1];
                tangent = points[2];
                p = points[3];
            }

            auto handlePoint = [&](ImVec2& p, int idx) -> bool
                {
                    static const float SIZE = 3;

                    ImVec2 cursor_pos = GetCursorScreenPos();
                    ImVec2 pos = transform(p);

                    SetCursorScreenPos(pos - ImVec2(SIZE, SIZE));
                    PushID(idx);
                    InvisibleButton("", ImVec2(2 * HANDLE_RADIUS, 2 * HANDLE_RADIUS));

                    bool is_selected = selected_point && *selected_point == point_idx + idx;
                    float thickness = is_selected ? 2.0f : 1.0f;
                    ImU32 col = IsItemActive() || IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered) : GetColorU32(ImGuiCol_PlotLines);

                    window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, SIZE), col, thickness);
                    window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, SIZE), col, thickness);
                    window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, -SIZE), col, thickness);
                    window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, -SIZE), col, thickness);

                    if (IsItemHovered()) hovered_idx = point_idx + idx;

                    bool changed = false;
                    if (IsItemActive() && IsMouseClicked(0))
                    {
                        if (selected_point) *selected_point = point_idx + idx;
                        window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
                        window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
                    }

                    if (IsItemHovered() || (IsItemActive() && IsMouseDragging(0)))
                    {
                        char tmp[64];
                        ImFormatString(tmp, sizeof(tmp), "%0.2f, %0.2f", p.x, p.y);
                        window->DrawList->AddText({ pos.x, pos.y - GetTextLineHeight() }, color_text, tmp);
                    }

                    if (IsItemActive() && IsMouseDragging(0))
                    {
                        pos.x = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
                        pos.y = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
                        pos += GetMouseDragDelta();
                        ImVec2 v = invTransform(pos);

                        p = v;
                        changed = true;
                    }
                    PopID();

                    SetCursorScreenPos(cursor_pos);
                    return changed;
                };

            auto handleTangent = [&](ImVec2& t, const ImVec2& p, int idx) -> bool
                {
                    static const float SIZE = 2;
                    static const float LENGTH = 18;

                    auto normalized = [](const ImVec2& v) -> ImVec2
                        {
                            float len = 1.0f / sqrtf(v.x * v.x + v.y * v.y);
                            return ImVec2(v.x * len, v.y * len);
                        };

                    ImVec2 cursor_pos = GetCursorScreenPos();
                    ImVec2 pos = transform(p);
                    ImVec2 tang = pos + normalized(ImVec2(t.x, -t.y)) * LENGTH;

                    SetCursorScreenPos(tang - ImVec2(SIZE, SIZE));
                    PushID(-idx);
                    InvisibleButton("", ImVec2(2 * HANDLE_RADIUS, 2 * HANDLE_RADIUS));

                    window->DrawList->AddLine(pos, tang, GetColorU32(ImGuiCol_PlotLines));

                    ImU32 col = IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered) : GetColorU32(ImGuiCol_PlotLines);

                    window->DrawList->AddLine(tang + ImVec2(-SIZE, SIZE), tang + ImVec2(SIZE, SIZE), col);
                    window->DrawList->AddLine(tang + ImVec2(SIZE, SIZE), tang + ImVec2(SIZE, -SIZE), col);
                    window->DrawList->AddLine(tang + ImVec2(SIZE, -SIZE), tang + ImVec2(-SIZE, -SIZE), col);
                    window->DrawList->AddLine(tang + ImVec2(-SIZE, -SIZE), tang + ImVec2(-SIZE, SIZE), col);

                    bool changed = false;
                    if (IsItemActive() && IsMouseDragging(0))
                    {
                        tang = GetIO().MousePos - pos;
                        tang = normalized(tang);
                        tang.y *= -1;

                        t = tang;
                        changed = true;
                    }
                    PopID();

                    SetCursorScreenPos(cursor_pos);
                    return changed;
                };

            PushID(point_idx);
            if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
            {
                window->DrawList->AddBezierCubic(
                    transform(p_prev),
                    transform(p_prev + tangent_last),
                    transform(p + tangent),
                    transform(p),
                    GetColorU32(ImGuiCol_PlotLines),
                    1.0f,
                    20);
                if (handleTangent(tangent_last, p_prev, 0))
                {
                    points[1] = ImClamp(tangent_last, ImVec2(0, -1), ImVec2(1, 1));
                    changed_idx = point_idx;
                }
                if (handleTangent(tangent, p, 1))
                {
                    points[2] = ImClamp(tangent, ImVec2(-1, -1), ImVec2(0, 1));
                    changed_idx = point_idx + 1;
                }
                if (handlePoint(p, 1))
                {
                    if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
                    if (point_idx < points_count - 2 && p.x >= points[6].x)
                    {
                        p.x = points[6].x - 0.001f;
                    }
                    points[3] = p;
                    changed_idx = point_idx + 1;
                }

            }
            else
            {
                window->DrawList->AddLine(transform(p_prev), transform(p), GetColorU32(ImGuiCol_PlotLines), 1.0f);
                if (handlePoint(p, 1))
                {
                    if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
                    if (point_idx < points_count - 2 && p.x >= points[2].x)
                    {
                        p.x = points[2].x - 0.001f;
                    }
                    points[1] = p;
                    changed_idx = point_idx + 1;
                }
            }
            if (point_idx == 0)
            {
                if (handlePoint(p_prev, 0))
                {
                    if (p.x <= p_prev.x) p_prev.x = p.x - 0.001f;
                    points[0] = p_prev;
                    changed_idx = point_idx;
                }
            }
            PopID();
        }

        SetCursorScreenPos(inner_bb.Min);

        InvisibleButton("bg", inner_bb.Max - inner_bb.Min);

        if (IsItemActive() && IsMouseDoubleClicked(0) && new_count && points_count < capacity)
        {
            ImVec2 mp = GetMousePos();
            ImVec2 new_p = invTransform(mp);
            ImVec2* points = (ImVec2*)values;

            if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
            {
                points[points_count * 3 + 0] = ImVec2(-0.2f, 0);
                points[points_count * 3 + 1] = new_p;
                points[points_count * 3 + 2] = ImVec2(0.2f, 0);
                ++*new_count;

                auto compare = [](const void* a, const void* b) -> int
                    {
                        float fa = (((const ImVec2*)a) + 1)->x;
                        float fb = (((const ImVec2*)b) + 1)->x;
                        return fa < fb ? -1 : (fa > fb) ? 1 : 0;
                    };

                qsort(values, points_count + 1, sizeof(ImVec2) * 3, compare);

            }
            else
            {
                points[points_count] = new_p;
                ++*new_count;

                auto compare = [](const void* a, const void* b) -> int
                    {
                        float fa = ((const ImVec2*)a)->x;
                        float fb = ((const ImVec2*)b)->x;
                        return fa < fb ? -1 : (fa > fb) ? 1 : 0;
                    };

                qsort(values, points_count + 1, sizeof(ImVec2), compare);
            }
        }

        if (hovered_idx >= 0 && IsMouseDoubleClicked(0) && new_count && points_count > 2)
        {
            ImVec2* points = (ImVec2*)values;
            --*new_count;
            if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
            {
                for (int j = hovered_idx * 3; j < points_count * 3 - 3; j += 3)
                {
                    points[j + 0] = points[j + 3];
                    points[j + 1] = points[j + 4];
                    points[j + 2] = points[j + 5];
                }
            }
            else
            {
                for (int j = hovered_idx; j < points_count - 1; ++j)
                {
                    points[j] = points[j + 1];
                }
            }
        }

        // ’Ç‰ÁLine
        {
            ImVec2* points = (ImVec2*)values;
            ImVec2 rangeMin = points[0];
            ImVec2 rangeMax = points[*new_count - 1];

            ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
            ItemSize(bb);

            ImVec2 a = transform({ curvePoint, -50 });
            ImVec2 b = transform({ curvePoint, 50 });

            window->DrawList->AddLine(a, b, 0x55000000, 2.0f);

            // smooth curve
            enum
            {
                smoothness = 256
            }; // the higher the smoother
            for (int i = 0; i <= (smoothness - 1); ++i)
            {
                float px = (i + 0) / float(smoothness);
                float qx = (i + 1) / float(smoothness);

                px = ImRemap(px, 0, 1, rangeMin.x, rangeMax.x);
                qx = ImRemap(qx, 0, 1, rangeMin.x, rangeMax.x);

                const float py = CurveValueSmooth(px, points_count, points);
                const float qy = CurveValueSmooth(qx, points_count, points);

                ImVec2 p = transform(ImVec2(px, py));
                ImVec2 q = transform(ImVec2(qx, qy));

                window->DrawList->AddLine(p, q, GetColorU32(ImGuiCol_PlotHistogram));
            }

        }

        if (hovered_point) *hovered_point = hovered_idx;
        // ƒ}ƒEƒX‚ª“à‘¤‚É‚¢‚é
        if (IsItemHovered()) *isHovered = true;

        EndChildFrame();
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

            // curve selector
        static const char* items[] = {
            "Custom",

            "Linear",          "Quad in",     "Quad out",   "Quad in  out",  "Cubic in",   "Cubic out",
            "Cubic in  out",   "Quart in",    "Quart out",  "Quart in  out", "Quint in",   "Quint out",
            "Quint in  out",   "Sine in",     "Sine out",   "Sine in  out",  "Expo in",    "Expo out",
            "Expo in  out",    "Circ in",     "Circ out",   "Circ in  out",  "Elastic in", "Elastic out",
            "Elastic in  out", "Back in",     "Back out",   "Back in  out",  "Bounce in",  "Bounce out",
            "Bounce in out",

            "Sine square",     "Exponential",

            "Schubring1",      "Schubring2",  "Schubring3",

            "SinPi2",          "Swing"
        };

        // buttons; @todo: mirror, smooth, tessellate
        if (ImGui::BeginPopupContextItem(label))
        {
            ImVec2* points = (ImVec2*)values;
            ImVec2 rangeMin = points[0];
            ImVec2 rangeMax = points[*new_count - 1];

            if (ImGui::Selectable("Flip"))
            {
                for (int i = 0; i < *new_count; ++i)
                {
                    const float yVal = 1.0f - ImRemap(points[i].y, rangeMin.y, rangeMax.y, 0, 1);
                    points[i].y = ImRemap(yVal, 0, 1, rangeMin.y, rangeMax.y);
                }
                *isSelectable = true;
            }
            if (ImGui::Selectable("Mirror"))
            {
                for (int i = 0, j = *new_count - 1; i < j; i++, j--)
                {
                    ImSwap(points[i], points[j]);
                }
                for (int i = 0; i < *new_count; ++i)
                {
                    const float xVal = 1.0f - ImRemap(points[i].x, rangeMin.x, rangeMax.x, 0, 1);
                    points[i].x = ImRemap(xVal, 0, 1, rangeMin.x, rangeMax.x);
                }
                *isSelectable = true;
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Presets"))
            {
                ImGui::PushID("curve_items");
                for (int row = 0; row < IM_ARRAYSIZE(items); ++row)
                {
                    if (ImGui::MenuItem(items[row]))
                    {
                        for (int i = 0; i < points_count; ++i)
                        {
                            const float px = i / float(points_count - 1);
                            const float py = float(tween::ease(row - 1, px));

                            points[i] = ImRemap(ImVec2(px, py), ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);
                        }
                    }
                }
                ImGui::PopID();
                ImGui::EndMenu();
                *isSelectable = true;
            }


            ImGui::EndPopup();
        }

        char buf[128];
        const char* str = label;

        return changed_idx;
    }

    void Label(const char* label) {
        ImGuiWindow* window = GetCurrentWindow();
        float fullWidth = GetContentRegionAvail().x;
        float itemWidth = fullWidth * 0.6f;
        ImVec2 textSize = CalcTextSize(label);
        ImRect textRect;
        textRect.Min = GetCursorScreenPos();
        textRect.Max = textRect.Min;
        textRect.Max.x += fullWidth - itemWidth;
        textRect.Max.y += textSize.y;

        AlignTextToFramePadding();
        textRect.Min.y += window->DC.CurrLineTextBaseOffset;
        textRect.Max.y += window->DC.CurrLineTextBaseOffset;

        ItemSize(textRect);
        if (ImGui::ItemAdd(textRect, window->GetID(label)))
        {
            RenderTextEllipsis(GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x,
                textRect.Max.x, label, nullptr, &textSize);

            if (textRect.GetWidth() < textSize.x && IsItemHovered())
                SetTooltip("%s", label);
        }
        SameLine();
        SetNextItemWidth(-1);
    }

    void HSplitter(const char* str_id, ImVec2* size)
    {
        ImVec2 screen_pos = GetCursorScreenPos();
        InvisibleButton(str_id, ImVec2(-1, 3));
        ImVec2 end_pos = screen_pos + GetItemRectSize();
        ImGuiWindow* win = GetCurrentWindow();
        ImVec4* colors = GetStyle().Colors;
        ImU32 color = GetColorU32(IsItemActive() || IsItemHovered() ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]);
        win->DrawList->AddRectFilled(screen_pos, end_pos, color);
        if (IsItemActive())
        {
            size->y = ImMax(1.0f, GetIO().MouseDelta.y + size->y);
        }
    }

};
