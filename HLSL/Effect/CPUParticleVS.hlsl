#include "CPUParticle.hlsli"

GS_IN main(VS_IN input)
{
    GS_IN output = (GS_IN) 0;
    output.Position = input.Position;
    output.Rotate = input.Rotate;
    output.Color = input.Color;
    output.Size = input.Size;
    output.Normal = input.Normal;
    output.Param = input.Param;
    return output;
}