#pragma once
#include<chrono>
#include<strsafe.h>
#include<d3d12.h>

void Log(std::ostream& os, const std::string& message);

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);


ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);