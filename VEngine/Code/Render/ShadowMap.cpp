#include "ShadowMap.h"
#include "Debug.h"
#include <cassert>
#include "Core.h"
#include "VMath.h"
#include "Components/Lights/DirectionalLightComponent.h"

ShadowMap::ShadowMap(ID3D11Device* device, int width_, int height_)
{
	width = width_;
	height = height_;

	// Viewport that matches the shadow map dimensions.
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width_);
	viewport.Height = static_cast<float>(height_);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going
	// to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = nullptr;
	HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	assert(depthMap);
	HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDSV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	assert(depthMap);
	HR(device->CreateShaderResourceView(depthMap, &srvDesc, &depthMapSRV));

	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	// Return 0 for points outside the light frustum
	// to put them in shadow.
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.BorderColor[0] = 0.f;
	sd.BorderColor[1] = 0.f;
	sd.BorderColor[2] = 0.f;
	sd.BorderColor[3] = 0.f;
	sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	
	HR(device->CreateSamplerState(&sd, &sampler));

	depthMap->Release();
}

ShadowMap::~ShadowMap()
{
	sampler->Release();
	depthMapDSV->Release();
	depthMapSRV->Release();
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* nullRTV = nullptr;
	dc->OMSetRenderTargets(1, &nullRTV, depthMapDSV);
	dc->ClearDepthStencilView(depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

XMMATRIX ShadowMap::GetLightPerspectiveMatrix()
{
	//Radius needs to be big otherwise the orthomatrix sort of wraps underneath and over the world
	float radius = 50.f;

	//Make the first directionalLight the main shadow projector for now
	if (DirectionalLightComponent::system.components.size() == 0)
	{
		throw;
	}

	auto light = DirectionalLightComponent::system.components[0];
	XMFLOAT3 center = light->transform.position;

	float l = center.x - radius;
	float b = center.y - radius;
	float n = center.z - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float f = center.z + radius;

	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	return P;
}

XMMATRIX ShadowMap::GetLightViewMatrix()
{
	if (DirectionalLightComponent::system.components.size() == 0)
	{
		throw;
	}

	auto light = DirectionalLightComponent::system.components[0];

	XMVECTOR eye = XMLoadFloat3(&light->transform.position);
	XMFLOAT3 forwardVec = light->GetForwardVector();
	XMVECTOR direction = XMLoadFloat3(&forwardVec);
	XMVECTOR focus = eye - (direction * 100.f);

	return XMMatrixLookAtLH(eye, direction, VMath::XMVectorUp());
}

XMMATRIX ShadowMap::GetLightTextureMatrix()
{
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	return T;
}

XMMATRIX ShadowMap::OutputMatrix()
{
	auto V = GetLightViewMatrix();
	auto P = GetLightPerspectiveMatrix();
	auto T = GetLightTextureMatrix();
	
	XMMATRIX S = V * P * T;
	return S;
}
