cbuffer cbPerPass : register(b0)
{
    float4x4 gProj;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    // Transform to World Space
    vout.PosH = mul(float4(vin.PosL, 1.0f), gProj);

    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color.rgba;
}