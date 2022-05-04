#pragma once
#include <vector>
#include <unordered_map>
#include <d3d11.h>
#include <dxgi1_6.h>
#include "RenderTypes.h"
#include "PipelineObjects.h"

struct MeshComponent;
struct Skeleton;

struct Renderer
{
	ID3D11Texture2D* backBuffer = nullptr;

	static const int swapchainCount = 2;
	static const UINT stride = sizeof(Vertex);
	static const UINT offset = 0;

	ID3D11Device* device;
	ID3D11DeviceContext* context;

	ID3D11RenderTargetView* rtvs[swapchainCount];
	
	ID3D11DepthStencilView* dsv;
	ID3D11InputLayout* inputLayout;

	//Rasterizer states
	std::unordered_map<std::string, RastState*> rastStateMap;
	std::unordered_map<std::string, BlendState*> blendStateMap;
	ID3D11RasterizerState* rastStateSolid;
	ID3D11RasterizerState* rastStateWireframe;
	ID3D11RasterizerState* rastStateNoBackCull;
	ID3D11RasterizerState* rastStateShadow;

	//Blendstates
	ID3D11BlendState* nullBlendState = nullptr;
	ID3D11BlendState* blendStateAlphaToCoverage = nullptr;

	//DXGI
	IDXGISwapChain3* swapchain;
	IDXGIFactory6* dxgiFactory;

	//Constant buffers and data
	ID3D11Buffer* cbMatrices;
	ID3D11Buffer* cbMaterial;
	ID3D11Buffer* cbLights;
	ID3D11Buffer* cbTime;
	ID3D11Buffer* cbMeshData;
	ID3D11Buffer* cbSkinningData;
	ID3D11Buffer* linesBuffer;

	//Viewport
	D3D11_VIEWPORT viewport;

	//Shadow maps
	struct ShadowMap* shadowMap;
	
	//Reflection
	ID3D11RenderTargetView* reflectionRTV;
	ID3D11ShaderResourceView* reflectionSRV;
	ID3D11Texture2D* reflectionTex;

	//Light probe buffers
	ID3D11RenderTargetView* lightProbeRTVs[6]; //Cubemap
	ID3D11ShaderResourceView* lightProbeSRV = nullptr;
	ID3D11Texture2D* lightProbeTexture = nullptr;

	//Post process
	ID3D11Texture2D* postBuffer = nullptr;
	ID3D11RenderTargetView* postRTV = nullptr;
	ID3D11ShaderResourceView* postSRV = nullptr;

private:
	//Queries for GPU profiling (Note that the queires are double buffered to deal with two frames for the GPU
	//being ahead of the GPU)
	ID3D11Query* frameStartQuery[2];
	ID3D11Query* frameEndQuery[2];
	ID3D11Query* timeDisjointQuery[2];
	int frameQueryIndex = 0;
	int framesCollected = -1;

	//Quality = 0 and Count = 1 are the 'default'
	DXGI_SAMPLE_DESC sampleDesc;

public:
	float frameTime;

	//GPU structures
	std::vector<DXGI_ADAPTER_DESC1> gpuAdaptersDesc;

	bool drawBoundingBoxes = false;
	bool drawTriggers = true;
	bool drawAllAsWireframe = false;

	void Init(void* window, int viewportWidth, int viewportHeight);
	void Tick();

private:
	void CreateFactory();
	void CreateDevice();
	void CreateSwapchain(HWND window);
	void CreateRTVAndDSV();
	void CreateInputLayout();
	void CreateRasterizerStates();
	void CreateBlendStates();
	void CreateQueries();
	void CreateConstantBuffers();
	void CreatePlanarReflectionBuffers();
	void CreateLightProbeBuffers();
	void CheckSupportedFeatures();
	void RenderShadowPass();
	void RenderMeshComponents();
	void RenderPlanarReflections();
	void RenderInstanceMeshComponents();
	void RenderBounds();
	void RenderCameraMeshes();
	void RenderLightMeshes();
	void RenderSkeletonBones();
	void RenderPolyboards();
	void RenderSpriteSheets();
	void RenderPostProcess();
	void AnimateSkeletalMesh(MeshComponent* mesh);
	void UpdateLights();
	void StartGPUQueries();
	void EndGPUQueries();
	void GetGPUQueryData();
	void MapBuffer(ID3D11Resource* resource, const void* src, size_t size);

	void DrawMesh(MeshComponent* mesh);

	//Inner render functions to set shader resources
	void SetNullRTV();
	void SetShadowData();
	void SetLightResources();
	void SetShadowResources();
	void SetReflectionResources();
	void SetMatricesFromMesh(MeshComponent* mesh);
	void SetShaderMeshData(MeshComponent* mesh);

public:
	void Present();
	void RenderSetup();
	void Render();

	//Renders sprites on the viewport as UI elements
	void RenderSpritesInScreenSpace();

	//Called as a 'baking' function once-off through editor
	void RenderLightProbeViews();

	void RenderParticleEmitters();
	void* GetSwapchain();
	float GetAspectRatio();
	float GetViewportWidth();
	float GetViewportHeight();
	void SetViewportWidthHeight(float width, float height);
	void ResizeSwapchain(int newWidth, int newHeight);

	//Write out viewport as image and save to file as an image.
	//Ref:https://github.com/Microsoft/DirectXTK/wiki/ScreenGrab
	void ScreenshotCapture();

private:
	void SetRenderPipelineStates(MeshComponent* mesh);
	void SetRenderPipelineStatesForShadows(MeshComponent* mesh);

	//Changes the global ambient param passed into shaders to change based on the day-night cycle in-game.
	XMFLOAT4 CalcGlobalAmbientBasedOnGameTime();

	void CreatePostProcessRenderTarget();
};

extern Renderer renderer;
