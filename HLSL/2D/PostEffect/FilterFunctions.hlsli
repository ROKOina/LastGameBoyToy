//--------------------------------------------
//	RGB色空間の数値からHSV色空間の数値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float3 RGB2HSV(float3 rgb)
{
    float3 hsv = 0;
	// V（明度） の最大値と最小値を求めましょう。
    float Vmax = max(rgb.r, max(rgb.g, rgb.b));

    float Vmin = min(rgb.r, min(rgb.g, rgb.b));

	// 最大値と最小値の差
    float delta = Vmax - Vmin;

	// V（明度） 一番強い色をV値にする
    hsv.z = Vmax;

	// S（彩度） 最大値と最小値の差を正規化して求める
    hsv.y = delta / Vmax;

	// H（色相） RGBのうち最大値と最小値の差から求める
    if (hsv.y > 0.0f)
    {

        if (Vmax == rgb.r)
        {
            hsv.x = 60 * ((rgb.g - rgb.b) / delta);
        }
        if (Vmax == rgb.g)
        {
            hsv.x = 60 * ((rgb.b - rgb.r) / delta) + 120;
        }
        if (Vmax == rgb.b)
        {
            hsv.x = 60 * ((rgb.r - rgb.g) / delta) + 240;
        }

        if (hsv.x < 0)
        {
            hsv.x += 360;
        }
    }
    return hsv;
}

//--------------------------------------------
//	HSV色空間の数値からRGB色空間の数値への変換関数
//--------------------------------------------
//hsv:HSV色空間の数値
float3 HSV2RGB(float3 hsv)
{
    float3 rgb = 0;
    if (hsv.y == 0)
    {
		// S（彩度）が0と等しいならば無色もしくは灰色
        rgb = hsv.zzz;
    }
    else
    {
		// 色環のH（色相）の位置とS（彩度）、V（明度）からRGB値を算出する
        float Vmax = hsv.z;
        float Vmin = Vmax - (hsv.y * Vmax);
        hsv.x %= 360; //	0～360に変換
        float Huei = (int) (hsv.x / 60);
        float Huef = hsv.x / 60 - Huei;
        float p = hsv.z * (1.0 - hsv.y);

        float q = hsv.z * (1.0 - hsv.y * Huef);

        float t = hsv.z * (1.0 - hsv.y * (1.0f - Huef));

        if (Huei == 0)
        {
            rgb.r = hsv.z;
            rgb.g = t;
            rgb.b = p;

        }
        else if (Huei == 1)
        {
            rgb.r = q;
            rgb.g = hsv.z;
            rgb.b = p;

        }
        else if (Huei == 2)
        {
            rgb.r = p;
            rgb.g = hsv.z;
            rgb.b = t;

        }
        else if (Huei == 3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = hsv.z;

        }
        else if (Huei == 4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = hsv.z;

        }
        else if (Huei == 5)
        {
            rgb.r = hsv.z;
            rgb.g = p;
            rgb.b = q;

        }
    }
    return rgb;
}

//--------------------------------------------
//	RGB色空間の数値から輝度値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float RGB2Luminance(float3 rgb)
{
    static const float3 LUMINANCE = float3(0.299f, 0.587f, 0.114f);
    return dot(LUMINANCE, rgb);
}
