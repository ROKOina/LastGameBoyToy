#ifndef	__COMMON_HLSLI__
#define	__COMMON_HLSLI__

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define BLACK_BORDER_POINT 3
#define BLACK_BORDER_LINEAR 4
#define BLACK_BORDER_ANISOTROPIC 5
#define WHITE_BORDER_POINT 6
#define WHITE_BORDER_LINEAR 7
#define WHITE_BORDER_ANISOTROPIC 8
SamplerState sampler_states[9] : register(s0);
SamplerComparisonState comparison_sampler_state : register(s9);

// ‰~Žü—¦
static const float PI = 3.141592653589;
// ƒKƒ“ƒ}’l
static const float GAMMA = 2.2;
// ŒÀ‚è‚È‚­0‚É‹ß‚¢’l
static const float EPSILON = 0.000001;

#endif // #endif __COMMON_HLSLI__