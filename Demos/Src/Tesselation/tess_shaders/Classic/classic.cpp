#include "hlsl.h"
#include "matrices.h"
#include "..\phong.h"

struct VS_Input {
    float3 vsCoord   : vsCoord;
    float3 vsNormal  : vsNormal;
};

struct VS_Output {
    float4 Pos    : SV_Position;
    float3 Normal : Normal;
    float3 ViewPos: ViewPos;
};

VS_Output VS(VS_Input In) {
    VS_Output Out;
    float4 crd = float4(In.vsCoord, 1.0);
    Out.Pos = mul(crd, VP_Matrix);
    Out.Normal = mul(In.vsNormal, (float3x3)V_Matrix);
    Out.ViewPos = mul(crd, V_Matrix).xyz;
    return Out;
}

//-------------------------------------------------
//Tesselation control shader
#define MAX_POINTS 3
struct HS_OutConstants {
    float Edges[3]        : SV_TessFactor;
    float Inside[1]       : SV_InsideTessFactor;
};

HS_OutConstants HS_ConstantFunc(InputPatch<VS_Output, MAX_POINTS> In) {
    HS_OutConstants Out;
    float tessC = 5.0;
    Out.Edges[0] = tessC;
    Out.Edges[1] = tessC;
    Out.Edges[2] = tessC;
    Out.Inside[0] = tessC;
    return Out;
}

struct HS_Output {
    float4 Pos       : SV_Position;
    float3 hsNormal  : hsNormal;
    float3 hsViewPos : hsViewPos;
};

struct HS_PathParams {
    uint i : SV_OutputControlPointID;
    uint PatchID : SV_PrimitiveID;    
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_ConstantFunc")]
HS_Output HS(InputPatch<VS_Output, MAX_POINTS> ip, HS_PathParams params) {
    VS_Output Out;
    Out = ip[(params.i+1)%3];
    return Out;
}

//--------------------------------------------------------
//Tesselation evaluation shader
struct DS_Output {
    float4 Pos       : SV_Position;
    float3 dsNormal  : dsNormal;
    float3 dsViewPos : dsViewPos;
};

[domain("tri")]
DS_Output DS(HS_OutConstants input, float3 uvwCoord : SV_DomainLocation, OutputPatch<HS_Output, MAX_POINTS> patch)
{
    DS_Output Out;
    float4 Pos     = uvwCoord.x * patch[0].Pos     + uvwCoord.y * patch[1].Pos     + uvwCoord.z * patch[2].Pos;
    float3 ViewPos = uvwCoord.x * patch[0].hsViewPos + uvwCoord.y * patch[1].hsViewPos + uvwCoord.z * patch[2].hsViewPos;
    float3 Normal  = uvwCoord.x * patch[0].hsNormal  + uvwCoord.y * patch[1].hsNormal  + uvwCoord.z * patch[2].hsNormal;
    Out.Pos = Pos;
    Out.dsViewPos = ViewPos;
    Out.dsNormal = Normal;

    return Out;
}

//--------------------------------------------------------
//Pixel shader

struct PS_Output {
    float4 Color : SV_Target;
};

PS_Output PS(DS_Output In) {
    PS_Output Out;
    float3 n = normalize(In.dsNormal);
    float4 Color = float4(1,1,1,1);
    Out.Color.xyz = Phong(0.0, 0.0, In.dsViewPos, n, Color.xyz);
    Out.Color.a = Color.a;
    //Out.Color = 1.0;
    return Out;
}