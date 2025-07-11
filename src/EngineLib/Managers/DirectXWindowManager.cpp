﻿#include "pch.h"
#include "DirectXWindowManager.h"

using namespace Microsoft::WRL;
using namespace DirectX;

DirectXWindowManager::DirectXWindowManager() { this->UnInit(); }
DirectXWindowManager::~DirectXWindowManager() { DESTRUCTOR_UNINIT(m_Initialized); }

bool DirectXWindowManager::Init(UINT16 windowWidth, UINT16 windowHeight, LPCWSTR windowTitle) {

	if (m_Initialized)
		return false;

	INIT_NEW_PTR(m_WindowInformationPtr, WindowInformation());
	INIT_NEW_PTR(m_DXRenderer, DXRenderer());

	DirectXWindowEventsManager::Get().Init(&m_Device); // Initialize DirectXWindowEventsManager
	InitializeWindow(windowWidth, windowHeight, windowTitle);
	InitializeDirectX3D();

	m_Initialized = true;

	return true;
}

void DirectXWindowManager::UnInit() {

	if (m_Device)
		FlushCommandQueue();

	UNINIT_PTR(m_WindowInformationPtr);
	UNINIT_PTR(m_DXRenderer);

	m_Initialized = false;
}

DirectXWindowManager& DirectXWindowManager::Get() {
	static DirectXWindowManager mInstance;
	return mInstance;
}

void DirectXWindowManager::Update() {

	if (m_lockMouseInWindow) {

		SetCursorPos(
			m_WindowInformationPtr->firstPixelPosition.x + m_WindowInformationPtr->halfWidth,
			m_WindowInformationPtr->firstPixelPosition.y + m_WindowInformationPtr->halfHeight
		);
	}

	XMMATRIX view = XMMatrixIdentity(); //Default matrix

	if (auto camera = CameraSystem::Get().GetSelectedCamera()) {
		XMVECTOR pos = XMLoadFloat3(&camera->m_Transform.GetPosition());
		XMVECTOR forwardVector = camera->m_Transform.GetForwardVector();
		XMVECTOR upVector = camera->m_Transform.GetUpVector();
		XMVECTOR target = pos + forwardVector;

		view = XMMatrixLookAtLH(pos, target, upVector); //Camera matrix
		XMStoreFloat4x4(&m_View, view);
	}
	else {
		
		float m_Phi = 0;
		float m_Theta = 0.0f;

		// Convert Spherical to Cartesian coordinates.
		float x = 1 * sinf(m_Phi) * cosf(m_Theta);
		float z = 1 * sinf(m_Phi) * sinf(m_Theta);
		float y = 1 * cosf(m_Phi);

		XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		view = XMMatrixLookAtLH(pos, target, upVector); //Camera matrix
		XMStoreFloat4x4(&m_View, view);
	}

	XMMATRIX proj = XMLoadFloat4x4(&m_Proj); //Perspective / orthographic matrix

	// Update the constant buffer with the latest worldViewProj matrix.
	for (auto& ri : m_OpaqueRitems) {
		ObjectConstants objConstants;
		XMMATRIX world = XMLoadFloat4x4(ri->pEntityWorldMatrix);
		XMMATRIX worldViewProj = world * view * proj;
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));

		if (!ri->ObjectCB) {
			std::cerr << "Erreur : ObjectCB est nullptr dans Update() pour ObjCBIndex = " << ri->ObjCBIndex << std::endl;
			throw std::runtime_error("Erreur : ObjectCB est nullptr dans Update() !");
		}
		ri->ObjectCB->CopyData(0, objConstants); // Met à jour l'UploadBuffer du RenderItem
	}
}


void DirectXWindowManager::Draw() {

	// Initialisation des commandes
	ThrowIfFailed(m_DirectCmdListAlloc->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

	// Configuration des buffers et viewport
	auto cbbv = CurrentBackBufferView();
	auto gdsv = GetDepthStencilView();
	m_CommandList->RSSetViewports(1, &mScreenViewport);
	m_CommandList->RSSetScissorRects(1, &mScissorRect);
	m_CommandList->OMSetRenderTargets(1, &cbbv, true, &gdsv);

	auto barrierBegin = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &barrierBegin);
	
	auto rtvHandle = CurrentBackBufferView();
	auto depthStencilView = GetDepthStencilView();

	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_CbvHeap};
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	
	for (auto& ri : m_OpaqueRitems) {

		m_CommandList->SetGraphicsRootSignature(m_RootSignature);
		m_CommandList->SetPipelineState(m_PSO);

		if (!ri->ObjectCB) {
			std::cerr << "[ERREUR] ObjectCB est nullptr pour ObjCBIndex = " << ri->ObjCBIndex << std::endl;
			continue;
		}
		if (!ri->ObjectCB->Resource()) {
			std::cerr << "[ERREUR] ObjectCB->Resource() est nullptr pour ObjCBIndex = " << ri->ObjCBIndex << std::endl;
			continue;
		}
		std::cerr << "[DEBUG] ObjCBIndex = " << ri->ObjCBIndex
			<< " - GPU Virtual Address: " << ri->ObjectCB->Resource()->GetGPUVirtualAddress() << std::endl;
		m_CommandList->SetGraphicsRootConstantBufferView(0, ri->ObjectCB->Resource()->GetGPUVirtualAddress());

		//Get the addresses of vertex and index buffer
		auto vbv = ri->Geo->VertexBufferView();
		auto ibv = ri->Geo->IndexBufferView();
		//1 vertex & index buffer per geometry
		//If you want draw 2 square in a raw you can use same addresses

		m_CommandList->IASetVertexBuffers(0, 1, &vbv);
		m_CommandList->IASetIndexBuffer(&ibv);
		m_CommandList->IASetPrimitiveTopology(ri->PrimitiveType);
		std::cerr << "[DEBUG] Draw : ObjCBIndex = " << ri->ObjCBIndex
			<< " - IndexCount = " << ri->IndexCount
			<< " - Position X = " << ri->pEntityWorldMatrix->m[3][0]
			<< " Y = " << ri->pEntityWorldMatrix->m[3][1]
			<< " Z = " << ri->pEntityWorldMatrix->m[3][2] << std::endl;
		m_CommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		//1 drawcall per object (not optimized)
	}

	// Finalisation
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &barrier);
	ThrowIfFailed(m_CommandList->Close());

	ID3D12CommandList* cmdsLists[] = {m_CommandList};
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	ThrowIfFailed(m_SwapChain->Present(0, 0));

	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}

bool DirectXWindowManager::InitializeWindow(const UINT16 windowWidth, const UINT16 windowHeight, const LPCWSTR windowTitle) {

	m_WindowInformationPtr->width = windowWidth;
	m_WindowInformationPtr->height = windowHeight;
	m_WindowInformationPtr->title = windowTitle;

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DirectXWindowEventsManager::MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT rect = {0, 0, m_WindowInformationPtr->width, m_WindowInformationPtr->height};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;

	m_WindowInformationPtr->mainWindowHandle = CreateWindow(L"MainWnd",
	    m_WindowInformationPtr->title,
	    WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    width,
	    height,
	    nullptr,
	    nullptr,
	    m_hAppInst,
	    nullptr);
if (!m_WindowInformationPtr->mainWindowHandle) {
		MessageBox(nullptr, L"CreateWindow Failed.", nullptr, 0);
		return false;
	}

	ShowWindow(m_WindowInformationPtr->mainWindowHandle, SW_SHOW);
	UpdateWindow(m_WindowInformationPtr->mainWindowHandle);
	return true;
}

int DirectXWindowManager::InitializeDirectX3D() {

#if defined(DEBUG) || defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		ID3D12Debug* debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

//Create FACTORY
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)));

//Create DEVICE
	CreateDevice(D3D_FEATURE_LEVEL_12_2, m_Device);

//Create FENCE
	CreateFence();
	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDescriptorsHeaps();
//CreateRenderTargetView();
	OnResize();

// Reset the command list to prep for initialization commands.
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

	BuildDescriptorHeaps();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
//SphereGeo(radius, numSubdivisions);
//GeometryGenerator::CreateGeoSphere(radius, numSubdivisions);
//BuildRenderItems();
	InitializePyramidTriangleGeo();
	InitializePyramidSquaredGeo();

	BuildConstantBuffers();
	BuildPSO();

	DXGeometry::GetGeometry(GeometryType::CUBE);

// Execute the initialization commands.
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = {m_CommandList};
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

// Wait until initialization is complete.
	FlushCommandQueue();

	return 0;
}


void DirectXWindowManager::CreateDevice(D3D_FEATURE_LEVEL minFeatureLevel, ID3D12Device*& device) {
	const HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		// default adapter
		minFeatureLevel,
		IID_PPV_ARGS(&device)
	);

	// Fallback to WARP device (DX11).
	if (FAILED(hardwareResult)) {

		IDXGIAdapter* pWarpAdapter;

		ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(
				pWarpAdapter,
				D3D_FEATURE_LEVEL_11_1,
				IID_PPV_ARGS(&device))
		);
	}
}

void DirectXWindowManager::CreateFence() {
	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DirectXWindowManager::CreateCommandObjects() {

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	ThrowIfFailed(m_Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_DirectCmdListAlloc)));
	ThrowIfFailed(m_Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc, // Associated command allocator
		nullptr, // Initial PipelineStateObject
		IID_PPV_ARGS(&m_CommandList)));
	// Start off in a closed state. This is because the first time we 
	// refer to the command list we will Reset it, and it needs to be 
	// closed before calling Reset.
	m_CommandList->Close();
}

void DirectXWindowManager::CreateSwapChain() {

	// Release the previous swapchain we will be recreating.
	if (m_SwapChain)
		m_SwapChain->Release();
	m_SwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_WindowInformationPtr->width;
	sd.BufferDesc.Height = m_WindowInformationPtr->height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; //MSAA disabled
	sd.SampleDesc.Quality = 0; //MSAA disabled
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = m_WindowInformationPtr->mainWindowHandle;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue, &sd, &m_SwapChain));
}

void DirectXWindowManager::CreateRtvAndDescriptorsHeaps() {
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvHeap)));
}

void DirectXWindowManager::CreateRenderTargetView() {

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SwapChainBufferCount; i++) {
		// Get the ith buffer in the swap chain.
		ThrowIfFailed(m_SwapChain->GetBuffer(
			i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		// Create an RTV to it.
		m_Device->CreateRenderTargetView(
			m_SwapChainBuffer[i],
			nullptr,
			rtvHeapHandle);
		// Next entry in heap.
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXWindowManager::CurrentBackBufferView() {
	// CD3DX12 constructor to offset to the RTV of the current back buffer.
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
		// handle start
		m_CurrBackBuffer,
		// index to offset
		m_RtvDescriptorSize
	); // byte size of descriptor
}

void DirectXWindowManager::CreateCommittedResource() {
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_WindowInformationPtr->width;
	depthStencilDesc.Height = m_WindowInformationPtr->height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = mDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1; //MSAA disabled
	depthStencilDesc.SampleDesc.Quality = 0; //MSAA disabled
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_Device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(&m_DepthStencilBuffer))
	);

	// Create descriptor to mip level 0 of entire resource using the
	// format of the resource.
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, GetDepthStencilView()); // Transition the resource from its initial state to be used as a depth buffer.

	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_CommandList->ResourceBarrier(1, &transition);
}

void DirectXWindowManager::BuildDescriptorHeaps() {
	UINT objCount = static_cast<UINT>(m_OpaqueRitems.size());
	UINT numDescriptors = (objCount + 1) * gNumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap)));
}

void DirectXWindowManager::BuildConstantBuffers() {

	if (!m_CbvHeap) {
		std::cerr << "[ERREUR] m_CbvHeap2 est nullptr avant la création du buffer !" << std::endl;
	}
	for (auto& item : m_OpaqueRitems) {
		if (!item->ObjectCB || !item->ObjectCB->Resource()) {
			std::cerr << "[ERREUR] Problème avec ObjectCB dans BuildConstantBuffers() !" << std::endl;
		}
	}

	for (auto& item : m_OpaqueRitems) {
		item->ObjectCB = new UploadBuffer<ObjectConstants>(m_Device, /*1*/(UINT)m_OpaqueRitems.size(), true);

		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = item->ObjectCB->Resource()->GetGPUVirtualAddress();
		int boxCBufIndex = 0;
		cbAddress += boxCBufIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		m_Device->CreateConstantBufferView(&cbvDesc, m_CbvHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void DirectXWindowManager::BuildRootSignature() {
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];


	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	slotRootParameter[0].InitAsConstantBufferView(0);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1,
	                                        slotRootParameter,
	                                        0,
	                                        nullptr,
	                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
	                                         D3D_ROOT_SIGNATURE_VERSION_1,
	                                         &serializedRootSig,
	                                         &errorBlob);

	if (errorBlob != nullptr) {
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
}

void DirectXWindowManager::BuildShadersAndInputLayout() {

	m_vsByteCode = d3dUtil::CompileShader(L"..\\..\\..\\src\\EngineLib\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0").Get();
	m_psByteCode = d3dUtil::CompileShader(L"..\\..\\..\\src\\EngineLib\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0").Get();

	m_InputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void DirectXWindowManager::BuildBoxGeometry() {
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

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_pBoxGeo = new MeshGeometry();
	m_pBoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_pBoxGeo->VertexBufferCPU));
	CopyMemory(m_pBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_pBoxGeo->IndexBufferCPU));
	CopyMemory(m_pBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_pBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                          m_CommandList,
	                                                          vertices.data(),
	                                                          vbByteSize,
	                                                          m_pBoxGeo->VertexBufferUploader);

	m_pBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                         m_CommandList,
	                                                         indices.data(),
	                                                         ibByteSize,
	                                                         m_pBoxGeo->IndexBufferUploader);

	m_pBoxGeo->VertexByteStride = sizeof(Vertex);
	m_pBoxGeo->VertexBufferByteSize = vbByteSize;
	m_pBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_pBoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = static_cast<UINT>(indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_pBoxGeo->DrawArgs["box"] = submesh;
	m_Geometries["box"] = m_pBoxGeo;
}

void DirectXWindowManager::InitializePyramidTriangleGeo() {
	std::array<Vertex, 4> vertices =
	{
		// Les 3 points de la base
		Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}), // 0
		Vertex({XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red)}), // 1
		Vertex({XMFLOAT3(0.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Green)}), // 2
		// Le sommet de la pyramide
		Vertex({XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(Colors::Blue)}) // 3
	};

	std::array<std::uint16_t, 12> indices =
	{
		// down face
		0, 1, 2,

		// face 1
		3, 1, 0,

		// face 2
		3, 2, 1,

		// face 3
		3, 0, 2
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_pPyramidTriangleGeo = new MeshGeometry();
	m_pPyramidTriangleGeo->Name = "PrismeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_pPyramidTriangleGeo->VertexBufferCPU));
	CopyMemory(m_pPyramidTriangleGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_pPyramidTriangleGeo->IndexBufferCPU));
	CopyMemory(m_pPyramidTriangleGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	//Envoie vers cg
	m_pPyramidTriangleGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                                      m_CommandList,
	                                                                      vertices.data(),
	                                                                      vbByteSize,
	                                                                      m_pPyramidTriangleGeo->VertexBufferUploader);

	m_pPyramidTriangleGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                                     m_CommandList,
	                                                                     indices.data(),
	                                                                     ibByteSize,
	                                                                     m_pPyramidTriangleGeo->IndexBufferUploader);

	m_pPyramidTriangleGeo->VertexByteStride = sizeof(Vertex);
	m_pPyramidTriangleGeo->VertexBufferByteSize = vbByteSize;
	m_pPyramidTriangleGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_pPyramidTriangleGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_pPyramidTriangleGeo->DrawArgs["prisme_triangle"] = submesh;

	m_Geometries["prisme_triangle"] = m_pPyramidTriangleGeo;
}

void DirectXWindowManager::InitializePyramidSquaredGeo() {
	std::array<Vertex, 5> vertices =
	{
		// v3 back side
		Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}), // 0
		Vertex({XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Red)}), // 1

		Vertex({XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Magenta)}), // 2
		Vertex({XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)}), // 3

		//sommet
		Vertex({XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Blue)}) // 4
	};

	std::array<std::uint16_t, 18> indices =
	{
		// face 0
		0, 1, 2,

		// face 1
		2, 3, 0,

		// face 2
		4, 1, 0,

		// face 3
		4, 2, 1,

		// face 4
		4, 3, 2,

		// face 5
		4, 0, 3


	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_pPyramidSquaredGeo = new MeshGeometry();
	m_pPyramidSquaredGeo->Name = "Prisme2Geo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_pPyramidSquaredGeo->VertexBufferCPU));
	CopyMemory(m_pPyramidSquaredGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_pPyramidSquaredGeo->IndexBufferCPU));
	CopyMemory(m_pPyramidSquaredGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_pPyramidSquaredGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                                     m_CommandList,
	                                                                     vertices.data(),
	                                                                     vbByteSize,
	                                                                     m_pPyramidSquaredGeo->VertexBufferUploader);

	m_pPyramidSquaredGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
	                                                                    m_CommandList,
	                                                                    indices.data(),
	                                                                    ibByteSize,
	                                                                    m_pPyramidSquaredGeo->IndexBufferUploader);

	m_pPyramidSquaredGeo->VertexByteStride = sizeof(Vertex);
	m_pPyramidSquaredGeo->VertexBufferByteSize = vbByteSize;
	m_pPyramidSquaredGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_pPyramidSquaredGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_pPyramidSquaredGeo->DrawArgs["prisme_triangle_carre"] = submesh;
	m_Geometries["prisme_triangle_carre"] = m_pPyramidSquaredGeo;
}

void DirectXWindowManager::BuildPSO() {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = {m_InputLayout.data(), (UINT)m_InputLayout.size()};
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_BackBufferFormat;
	psoDesc.SampleDesc.Count = 1; //m4xMsaaState ? 4 : 1;  //MSAA disabled  
	psoDesc.SampleDesc.Quality = 0; //m4xMsaaState ? (m4xMsaaQuality - 1) : 0;  //MSAA disabled
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
}

void DirectXWindowManager::FlushCommandQueue() {
	std::cerr << "[DEBUG] Début de FlushCommandQueue()" << std::endl;
	// Advance the fence value to mark commands up to this fence point.
	m_CurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence, m_CurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_Fence->GetCompletedValue() < m_CurrentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

bool DirectXWindowManager::CheckMSAASupport() {

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(m_Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels))
	);

	return true;
}


void DirectXWindowManager::OnResize() {
	assert(m_Device);
	assert(m_SwapChain);
	assert(m_DirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i) {
		if (m_SwapChainBuffer[i]) {
			m_SwapChainBuffer[i]->Release();
			m_SwapChainBuffer[i] = nullptr;
		}
		if (m_DepthStencilBuffer) {
			m_DepthStencilBuffer->Release();
			m_DepthStencilBuffer = nullptr;
		}
	}

	// Resize the swap chain.
	ThrowIfFailed(m_SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		m_WindowInformationPtr->width, m_WindowInformationPtr->height,
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_CurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++) {
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i], nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_WindowInformationPtr->width;
	depthStencilDesc.Height = m_WindowInformationPtr->height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1; //NO MSAA
	depthStencilDesc.SampleDesc.Quality = 0; //NO MSAA
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	auto c = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_Device->CreateCommittedResource(
		&c,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(&m_DepthStencilBuffer)));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &dsvDesc, GetDepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	auto d = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_CommandList->ResourceBarrier(1, &d);

	// Execute the resize commands.
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = {m_CommandList};
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(m_WindowInformationPtr->width);
	mScreenViewport.Height = static_cast<float>(m_WindowInformationPtr->height);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = {.left= 0, .top= 0, .right= m_WindowInformationPtr->width, .bottom= m_WindowInformationPtr->height};
	
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, this->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);

	m_WindowInformationPtr->UpdateFirstPixelPosition();
}

void DirectXWindowManager::DrawEntity(const GeometryType& geoType, const Entity& entity) {
	RenderItem* renderItem = new RenderItem();
	Transform& entityTransform = entity.m_Transform;
	renderItem->pEntityWorldMatrix = &entityTransform.mWorld;
	renderItem->ObjCBIndex = static_cast<UINT>(m_OpaqueRitems.size());
	renderItem->Geo = &DXGeometry::GetGeometry(geoType);
	renderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	const char* geoDrawArgs = DXGeometry::GetGeoDrawArgs(geoType);
	renderItem->IndexCount = renderItem->Geo->DrawArgs[geoDrawArgs].IndexCount;
	renderItem->StartIndexLocation = renderItem->Geo->DrawArgs[geoDrawArgs].StartIndexLocation;
	renderItem->BaseVertexLocation = renderItem->Geo->DrawArgs[geoDrawArgs].BaseVertexLocation;
	renderItem->ObjectCB = new UploadBuffer<ObjectConstants>(m_Device, 1, true);
	if (!renderItem->ObjectCB || !renderItem->ObjectCB->Resource()) {
		std::cerr << "[ERREUR] ObjectCB non initialisé dans BuildRenderItems() !" << std::endl;
	}
	else {
		std::cerr << "[DEBUG] ObjectCB bien créé pour ObjCBIndex = " << renderItem->ObjCBIndex << std::endl;
	}

	m_OpaqueRitems.push_back(renderItem);
}
