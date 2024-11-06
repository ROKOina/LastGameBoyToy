/*
 * Xorshift32を用いて32bitの擬似乱数を生成する
 */
uint XOrShift32(uint value)
{
    value = value ^ (value << 13);
    value = value ^ (value >> 17);
    value = value ^ (value << 5);
    return value;
}

//ランダム関数
float rand(float2 co) //引数はシード値と呼ばれる　同じ値を渡せば同じものを返す
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// 1次元のランダムな値を算出する
float random(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float random(float2 seed)
{
    uint h = XOrShift32(asuint(seed.x));
    h = XOrShift32(h ^ asuint(seed.y));
    return asfloat((h & 0x007FFFFF) | 0x40000000) - 3.0;
}