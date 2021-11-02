#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

//Basic tutorial for shadow mapping
//REF: https://takinginitiative.wordpress.com/2011/05/15/directx10-tutorial-10-shadow-mapping/
//REF: https://takinginitiative.wordpress.com/2011/05/25/directx10-tutorial-10-shadow-mapping-part-2/

//Cool presentation on Silhouette maps. Old (2004), but nice to look at for ideas
//REF: https://jankautz.com/courses/ShadowCourse/04-SilhouetteMap.pdf

using namespace DirectX;

//All this is stolen from Luna's book
struct ShadowMap
{
	int width = 0;
	int height = 0;

	//Radius needs to be big otherwise the orthomatrix sort of wraps underneath and over the world
	float levelRadius = 0.f;

	ID3D11ShaderResourceView* depthMapSRV = nullptr;;
	ID3D11DepthStencilView* depthMapDSV = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	D3D11_VIEWPORT viewport;

	ShadowMap(ID3D11Device* device, int width_, int height_);
	~ShadowMap();
	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);

	XMMATRIX GetLightPerspectiveMatrix();
	XMMATRIX GetLightViewMatrix();
	XMMATRIX GetLightTextureMatrix();
	XMMATRIX OutputMatrix();
};
