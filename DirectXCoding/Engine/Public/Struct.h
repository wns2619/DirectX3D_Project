#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#pragma warning(disable : 26495)

enum class LayoutType { POS_NOR_TEX_TAN, POS_NOR_TEX, POS_TEX_FINDEX, POS_TEX, POS_COL, POS, PARTICLE, NONE };


namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN,  WINMODE_END };

		HWND	_hwnd;
		WINMODE	_winMode;
		unsigned int	iWinSizeX, iWinSizeY;
	}GRAPHIC_DESC;


	/* VertexType Struct */
	struct ENGINE_DLL VertexPos
	{
		Vec3 position;

		static const uint32 VertexPosElementCount = 1;
	};


	struct ENGINE_DLL VertexPosTexFrustumIndex
	{
		Vec3 position;
		Vec2 uv;
		uint32 frustumIndex;

		static const uint32 VertexPosTexFrustumIndexElementCount = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[VertexPosTexFrustumIndexElementCount];
	};

	struct VertexPosCol
	{
		Vec3 position;
		Vec4 color;

		static const unsigned int VertexPosColElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[VertexPosColElementCount];
	};


	struct ENGINE_DLL VertexTextureData
	{
		Vec3 position = { 0,0,0, };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];

	};


	struct ENGINE_DLL VertexTextureNormalData
	{
		Vec3 position = { 0,0,0 };
		Vec3 normal = { 0,0,0 };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};
	
	struct ENGINE_DLL VertexPosNormTexTan
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 bitangent;
		XMFLOAT2 texCoord;

		static const uint32 numElements = 5;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};

	struct ENGINE_DLL VertexParticle
	{
		XMFLOAT3 position;
		XMFLOAT3 velocity;
		XMFLOAT2 size;
		float rotation; // radiance
		float age;
		UINT type;
		UINT maxId;

		static const uint32 numElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};


	/* ConstantBuffer struct */

    struct ENGINE_DLL VS_WVP_CBUFFER
    {
        XMMATRIX wvp;
        XMMATRIX worldMatrix;
        XMMATRIX normalMatrix;
        VS_WVP_CBUFFER()
        {
            wvp = XMMatrixIdentity();
            worldMatrix = XMMatrixIdentity();
            normalMatrix = XMMatrixIdentity();
        }
    };

    struct ENGINE_DLL VS_VP_MATRIX_CBUFFER
    {
        XMMATRIX viewMatrix;
        XMMATRIX projMatrix;
    };

    struct ENGINE_DLL VS_SKYBOX_MATRIX_CBUFFER
    {
        XMMATRIX vpMatrix;
        VS_SKYBOX_MATRIX_CBUFFER()
        {
            vpMatrix = XMMatrixIdentity();
        }
    };

    struct ENGINE_DLL PROCEDURAL_SKY_CBUFFER
    {
        XMFLOAT3 skyColor = XMFLOAT3(0.05f, 0.09f, 0.2f);
        float skyExponent = 5.f;
        XMFLOAT3 skyNightColor = XMFLOAT3(0.02f, 0.01f, 0.03f);
        float skyNightExponent = 6.f;

        XMFLOAT3 horizonColor = XMFLOAT3(0.4f, 0.4f, 0.4f);
        float intensity = 1.f;
        XMFLOAT3 horizonNightColor = XMFLOAT3(0.025f, 0.02f, 0.028f);
        BOOL useSunMoonColorforHorizon = false;

        XMFLOAT3 groundColor = XMFLOAT3(0.f, 0.206f, 0.08f);
        float groundExponent = 5.f;
        XMFLOAT3 groundNightColor = XMFLOAT3(0.f, 0.02f, 0.01f);
        float groundNightExponent = 1.f;

        float sunIntensity = 5.f;
        float sunExponent = 1.f;
        float sunRadiusA = 0.001f;
        float sunRadiusB = 0.03f;

        XMFLOAT3 sunSetRiseColor = XMFLOAT3(0.5f, 0.2f, 0.1f);

        float starsUVScale = 1.f;
        float starsIntensity = 1.f;

        float moonIntensity = 2.f;
        float moonRadiusA = 0.01f;
        float moonRadiusB = 0.02f;

        XMFLOAT3 moonColor = XMFLOAT3(0.25f, 0.32f, 0.5f);
        float moonExponent = 1.f;
    };

    struct ENGINE_DLL Light
    {
        XMFLOAT4	position;

        XMFLOAT3    direction = XMFLOAT3(0.f, 0.f, 0.f);
        float       intensity = 1.f;

        XMFLOAT3    color = XMFLOAT3(1.f, 1.f, 1.f);
        float       range = 1.f;

        XMFLOAT2    spotAngles = XMFLOAT2(1.f, 1.f);
        int         type;
        BOOL        enabled = true;
    };

    const UINT LIGHT_CAP = 40;

    struct ENGINE_DLL PS_LIGHT_BUFFER
    {
        Light lights[LIGHT_CAP];
        UINT nrOfLights;
        float enviormentDiffContribution = 1.f;// 0.03f;
        float enviormentSpecContribution = 1.f;//0.04f;
        BOOL volumetricSunScattering = TRUE;
        BOOL fog = TRUE;
        BOOL procederualSky = TRUE;
        XMFLOAT2 pad;
    };

    struct ENGINE_DLL SKY_LIGHT_DATA_CBUFFER
    {
        XMFLOAT3 direction;
        float intensity;
        XMFLOAT3 color;
        int moonOrSun; // 0 = Moon, 1 = Sun
        BOOL castingShadow;
        XMFLOAT3 ambientColor; // used when procederualSky is on
    };

    struct ENGINE_DLL LENS_FLARE_CBUFFER
    {
        XMVECTOR skyLightPosition; // In Screen Space
        XMFLOAT2 screenDimensions;
        XMFLOAT2 pad;
    };

    struct ENGINE_DLL LightHelper
    {
        XMFLOAT3 rotationDeg;
        XMFLOAT2 spotAngles;
    };

    struct ENGINE_DLL PS_COLOR_ANIMATION_BUFFER
    {
        float colorOpacity = 1.f;
        XMFLOAT3 pad;
    };

    struct ENGINE_DLL PARTICLE_CBUFFER
    {
        XMMATRIX viewMatrix;
        XMMATRIX projMatrix;
        XMFLOAT3 camPosition;
        float gameTime;
        XMFLOAT3 emitPosition = XMFLOAT3(0.f, 2.f, 0.f);
        float deltaTime;
        float maxParticles;
        XMFLOAT3 pad;
    };

    struct ENGINE_DLL PARTICLE_STYLE
    {
        XMFLOAT3 colorBegin = XMFLOAT3(1.f, 1.f, 1.f);
        float colorBias = 0.2f;
        XMFLOAT3 colorEnd = XMFLOAT3(.9f, .9f, .9f);
        float colorIntensity = 1.f;
        float scaleVariationMax = 0.2f;
        float rotationVariationMax = XM_PIDIV4;
        float lifetime = 1.f;
        BOOL useNoise = true;
        XMFLOAT3 emitDirection = XMFLOAT3(0.f, 1.f, 0.f);
        float emitInterval = 0.1f;
        BOOL randomizePosition = false;
        XMFLOAT3 randomizePosBounds = XMFLOAT3(0.f, 0.f, 0.f);
        BOOL randomizeDirection = false;
        BOOL dieOnCollition = false;
        BOOL fadeInAndOut = false;
        UINT idInterval = 0; // Used to reset particle ID range
    };

    const int MAX_BLUR_RADIUS = 15;
    struct ENGINE_DLL CS_BLUR_CBUFFER
    {
        XMMATRIX projectionMatrix;
        int radius;
        BOOL direction;
        XMFLOAT2 pad;
        alignas(16) float weights[MAX_BLUR_RADIUS]; // BLUR_RADIUS + 1 * 4 bytes
    };

    struct ENGINE_DLL CS_DOWNSAMPLE_CBUFFER
    {
        XMFLOAT4 threshold; // x = threshold, y = threshold - knee, z = knee * 2, w = 0.25 / knee
        XMFLOAT2 mipDimensions;
        float mipLevel;
        float exposure = 0.f;
    };

    struct ENGINE_DLL CS_UPSAMPLE_CBUFFER
    {
        XMFLOAT2 mipDimensions;
        float mipLevel;
        float pad;
    };

    struct ENGINE_DLL PS_HISTOGRAM_CBUFFER
    {
        UINT texWidth;
        UINT texHeight;
        float minLogLuminance = -2.f; // min
        float oneOverLogLuminanceRange = 1.f / (2.f + 20.f); // 1 / (abs min + range)
    };

    struct ENGINE_DLL PS_HISTOGRAM_AVERAGING_CBUFFER
    {
        UINT pixelCount;
        float minLogLuminance = -1.5f; // min
        float logLuminanceRange = 20.f; // range
        float deltaTime;
        float tau = 0.3f;
        XMFLOAT3 pad;
    };

    struct ENGINE_DLL PS_TONEMAP_CBUFFER
    {
        // Should be expressed as a relative expsure value (-2, -1, 0, +1, +2 )
        float Exposure = 0.0f;

        // Gamma, 2.2 by default
        float Gamma = 2.2f;

        // ACES Filmic
        // See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
        float ACESss = 0.5f; // Shoulder strength
        float ACESls = 0.1f; // Linear strength
        float ACESla = 0.01f; // Linear angle
        float ACESts = 0.55f; // Toe strength
        float ACEStn = 0.02f; // Toe Numerator
        float ACEStd = 0.3f; // Toe denominator

        // Note ACEStn/ACEStd = Toe angle.
        float LinearWhite = 12.0f;

        // Padding
        XMFLOAT3 pad;
    };

    struct ENGINE_DLL DS_TESSELLATION_CBUFFER
    {
        float tessFactor;
        XMFLOAT3 pad;
    };

}


#endif // Engine_Struct_h__
