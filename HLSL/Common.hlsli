#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define BORDER_POINT 3
#define BORDER_LINEAR 4
#define BORDER_ANISOTROPIC 5
SamplerState sampler_states[6] : register(s0);

// 円周率
static const float PI = 3.141592653589;
// ガンマ値
static const float GAMMA = 2.2;
// 限りなく0に近い値
static const float EPSILON = 0.000001;