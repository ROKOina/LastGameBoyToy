#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define BORDER_POINT 3
#define BORDER_LINEAR 4
#define BORDER_ANISOTROPIC 5
SamplerState sampler_states[6] : register(s0);

// �~����
static const float PI = 3.141592653589;
// �K���}�l
static const float GAMMA = 2.2;
// ����Ȃ�0�ɋ߂��l
static const float EPSILON = 0.000001;