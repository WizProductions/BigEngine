#include "pch.h"
#include "DXGeometry.h"

#include <ranges>
DXGeometry::DXGeometry() { this->UnInit(); }
DXGeometry::~DXGeometry() { DESTRUCTOR_UNINIT(m_Initialized); }
bool DXGeometry::Init() {

	if (m_Initialized)
		return false;

	ret m_Initialized = true;
}

void DXGeometry::UnInit() {

	for (auto geometry : m_Geometries | std::views::values) {
		delete geometry;
	}
	m_Geometries.clear();
	
	m_Initialized = false;
}

MeshGeometry* DXGeometry::CreateGeometry(GeometryType geometryType) {

	MeshGeometry* meshGeometry = new MeshGeometry();

	if (auto func = geometryCreateFunctions[static_cast<int>(geometryType)])
		func((*meshGeometry));
	else
		_ASSERT(false);

	return meshGeometry;
}

MeshGeometry& DXGeometry::GetGeometry(GeometryType geometryType) {

	if (m_Geometries.contains(geometryType)) {
		return *m_Geometries[geometryType];
	}
	
	m_Geometries[geometryType] = CreateGeometry(geometryType);

	return *m_Geometries[geometryType];
}

const char* DXGeometry::GetGeoDrawArgs(GeometryType geometryType) {

	switch (geometryType) {
	case GeometryType::CUBE:
		ret "box";
	case GeometryType::PYRAMID_SQUARE:
		ret "pyramid_square";
	case GeometryType::PYRAMID_TRIANGLE:
		break;
	case GeometryType::VALUE_COUNT:
	default: std::cerr << "[ERROR] DXGeometry::GetGeoDrawArgs() : Unknown geometryType!" << std::endl;
		break;
	}

}

void DXGeometry::InitCubeGeo(MeshGeometry& geometry) {

	std::array<Vertex, 8> vertices =
	{
		Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}),
		Vertex({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black)}),
		Vertex({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red)}),
		Vertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)}),
		Vertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue)}),
		Vertex({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)}),
		Vertex({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan)}),
		Vertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta)})
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	constexpr UINT vbByteSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	constexpr UINT ibByteSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);
	
	geometry.Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geometry.VertexBufferCPU));
	CopyMemory(geometry.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geometry.IndexBufferCPU));
	CopyMemory(geometry.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geometry.VertexBufferGPU = d3dUtil::CreateDefaultBuffer(DirectXWindowManager::m_Device,
	                                                        DirectXWindowManager::m_CommandList,
	                                                        vertices.data(),
	                                                        vbByteSize,
	                                                        geometry.VertexBufferUploader);

	geometry.IndexBufferGPU = d3dUtil::CreateDefaultBuffer(DirectXWindowManager::m_Device,
	                                                       DirectXWindowManager::m_CommandList,
	                                                       indices.data(),
	                                                       ibByteSize,
	                                                       geometry.IndexBufferUploader);

	geometry.VertexByteStride = sizeof(Vertex);
	geometry.VertexBufferByteSize = vbByteSize;
	geometry.IndexFormat = DXGI_FORMAT_R16_UINT;
	geometry.IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = static_cast<UINT>(indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geometry.DrawArgs["box"] = submesh;
	m_Geometries[GeometryType::CUBE] = &geometry;
}
