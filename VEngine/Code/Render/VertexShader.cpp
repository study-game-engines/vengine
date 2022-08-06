#include "vpch.h"
#include "VertexShader.h"
#include "RenderUtils.h"
#include "Debug.h"

void VertexShader::Create(const wchar_t* filename)
{
    ReadData(filename);

    HR(RenderUtils::device->CreateVertexShader(
        GetByteCodeData(),
        GetByteCodeSize(),
        nullptr,
        GetShaderAddress()));
}

void VertexShader::Reset()
{
    shader.Reset();
    byteCode.clear();
}
