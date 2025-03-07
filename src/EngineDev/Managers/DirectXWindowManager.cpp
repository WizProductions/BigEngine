#include "pch.h"
#include "DirectXWindowManager.h"
#include "ApplicationManager.h"
#include "DirectXFiles/GameTimer.h"
#include "DirectXFiles/d3dUtil.h"
#include "DirectXFiles/UploadBuffer.h"
//#include "Utils/GeometryGenerator.h"

using namespace Microsoft::WRL;
using namespace DirectX;

DirectXWindowManager::DirectXWindowManager() { this->UnInit(); }
DirectXWindowManager::~DirectXWindowManager() { DESTRUCTOR_UNINIT(m_Initialized); }

const int gNumFrameResources = 1;

bool DirectXWindowManager::Init(UINT16 windowWidth, UINT16 windowHeight, LPCWSTR windowTitle) {

	if (m_Initialized)
		return false;

	ApplicationManager& app = *ApplicationManager::Get();
	INIT_PTR(m_TimerPtr, &app.GetTimer());
	INIT_PTR(m_AppIsPausedPtr, &app.AppIsPaused());

	InitializeWindow(windowWidth, windowHeight, windowTitle);
	InitializeDirectX3D();

	m_Initialized = true;

	return true;
}

void DirectXWindowManager::UnInit() {

	if (m_Device)
		FlushCommandQueue();

	m_Initialized = false;
}

DirectXWindowManager& DirectXWindowManager::Get() {
	static DirectXWindowManager mInstance;
	return mInstance;
}



void DirectXWindowManager::Update() {

	std::cerr << "[DEBUG] Update() appelé" << std::endl;

	// Convert Spherical to Cartesian coordinates.
	float x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	float z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	float y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);

	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	//ObjectConstants objConstants;
	//XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	//ObjectCB->CopyData(0, objConstants);
	for (auto& ri : m_OpaqueRitems) {
		ObjectConstants objConstants;
		XMMATRIX world = XMLoadFloat4x4(&ri->World);
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
	
	std::cerr << "[DEBUG] Début de Draw()" << std::endl;
	std::cerr << "[DEBUG] Nombre d'objets opaques à dessiner : " << m_OpaqueRitems.size() << std::endl;
	// Initialisation des commandes
	ThrowIfFailed(m_DirectCmdListAlloc->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr))

	// Configuration des buffers et viewport
	auto cbbv = CurrentBackBufferView();
	auto gdsv = GetDepthStencilView();
	m_CommandList->RSSetViewports(1, &mScreenViewport);
	m_CommandList->RSSetScissorRects(1, &mScissorRect);
	m_CommandList->OMSetRenderTargets(1, &cbbv, true, &gdsv);

	auto barrierBegin = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &barrierBegin);
	/*int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CbvHeap2->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, mCbvSrvUavDescriptorSize);
	m_CommandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

	m_CommandList.Get(), m_OpaqueRitems;*/

	// Effacer le render target et le depth/stencil.
	auto rtvHandle = CurrentBackBufferView();
	auto depthStencilView = GetDepthStencilView();

	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap2 };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// Swap the back and front buffers
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

	// Dessiner chaque objet
	for (auto& ri : m_OpaqueRitems)
	{
		//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
		//cbAddress += ri->ObjCBIndex * d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		//set pipeline state
		//pso = psoDesc
		//ID3D12PipelineState *pPipelineState

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


	//	m_CommandList->IASetVertexBuffers(0, 1, &vbv);
	//	m_CommandList->IASetIndexBuffer(&ibv);
		m_CommandList->IASetPrimitiveTopology(ri->PrimitiveType);
		std::cerr << "[DEBUG] Draw : ObjCBIndex = " << ri->ObjCBIndex
			<< " - IndexCount = " << ri->IndexCount
			<< " - Position X = " << ri->World.m[3][0]
			<< " Y = " << ri->World.m[3][1]
			<< " Z = " << ri->World.m[3][2] << std::endl;
		m_CommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}

	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}

LRESULT DirectXWindowManager::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.

	return DirectXWindowManager::Get().MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT DirectXWindowManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			*m_AppIsPausedPtr = true;
			m_TimerPtr->Stop();
		}
		else
		{
			*m_AppIsPausedPtr = false;
			m_TimerPtr->Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		DirectXWindowManager::WINDOW_WIDTH = LOWORD(lParam);
		DirectXWindowManager::WINDOW_HEIGHT = HIWORD(lParam);
		if (m_Device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				*m_AppIsPausedPtr = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				*m_AppIsPausedPtr = false;
				m_Minimized = false;
				m_Maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (m_Minimized)
				{
					*m_AppIsPausedPtr = false;
					m_Minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_Maximized)
				{
					*m_AppIsPausedPtr = false;
					m_Maximized = false;
					OnResize();
				}
				else if (m_Resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		/*mAppPaused = true;
		mResizing  = true;
		mTimer.Stop();*/
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		/*mAppPaused = false;
		mResizing  = false;
		mTimer.Start();
		OnResize();*/
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
			//Set4xMsaaState(!m4xMsaaState);

			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool DirectXWindowManager::InitializeWindow(const UINT16 windowWidth, const UINT16 windowHeight, const LPCWSTR windowTitle) {

	WINDOW_WIDTH = windowWidth;
	WINDOW_HEIGHT = windowHeight;
	WINDOW_TITLE = windowTitle;

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;

	m_hMainWnd = CreateWindow(L"MainWnd", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", nullptr, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);
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

XMStoreFloat4x4(&m_World, XMMatrixIdentity());

BuildDescriptorHeaps();
BuildRootSignature();
BuildShadersAndInputLayout();
BuildBoxGeometry();
BuildPrismeTriangleGeo_btriangle();
BuildPrismeTriangleGeo_bcarre();
SphereGeo();
/*//m_Geometries["box"] = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f);
//m_Geometries["prisme_triangle"] = GeometryGenerator::CreatePrismTriangle();
//m_Geometries["prisme_carre"] = GeometryGenerator::CreatePrismSquareBase();
//m_Geometries["geosphere"] = GeometryGenerator::CreateGeoSphere(20, 20, 1.0f);*/
BuildRenderItems();
BuildConstantBuffers();
BuildPSO();

std::cerr << "[DEBUG] m_OpaqueRitems contient " << m_OpaqueRitems.size() << " éléments." << std::endl;





// Execute the initialization commands.
ThrowIfFailed(m_CommandList->Close());
ID3D12CommandList* cmdsLists[] = { m_CommandList };
m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

// Wait until initialization is complete.
FlushCommandQueue();

return 0;
}


void DirectXWindowManager::CreateDevice(D3D_FEATURE_LEVEL minFeatureLevel, ID3D12Device*& device) {
	const HRESULT hardwareResult = D3D12CreateDevice(
		nullptr, // default adapter
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
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; //MSAA disabled
	sd.SampleDesc.Quality = 0; //MSAA disabled
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = m_hMainWnd;
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

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		// Get the ith buffer in the swap chain.
		ThrowIfFailed(m_SwapChain->GetBuffer(
			i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		// Create an RTV to it.
		m_Device->CreateRenderTargetView(
			m_SwapChainBuffer[i], nullptr, rtvHeapHandle);
		// Next entry in heap.
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXWindowManager::CurrentBackBufferView() {
	// CD3DX12 constructor to offset to the RTV of the current back buffer.
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),	// handle start
		m_CurrBackBuffer, // index to offset
		m_RtvDescriptorSize
	); // byte size of descriptor
}

void DirectXWindowManager::CreateCommittedResource() {
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = WINDOW_WIDTH;
	depthStencilDesc.Height = WINDOW_HEIGHT;
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

void DirectXWindowManager::BuildDescriptorHeaps()
{
	UINT objCount = (UINT)m_OpaqueRitems.size();

	UINT numDescriptors = (objCount + 1) * gNumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap2)));

	/*D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap2)));*/
}

void DirectXWindowManager::BuildConstantBuffers() {

	if (!m_CbvHeap2) {
		std::cerr << "[ERREUR] m_CbvHeap2 est nullptr avant la création du buffer !" << std::endl;
	}
	for (auto& item : m_OpaqueRitems) {
		if (!item->ObjectCB || !item->ObjectCB->Resource()) {
			std::cerr << "[ERREUR] Problème avec ObjectCB dans BuildConstantBuffers() !" << std::endl;
		}
	}
	if (m_OpaqueRitems.empty()) {
		throw std::runtime_error("Erreur : m_OpaqueRitems est vide lors de l'allocation d'ObjectCB !");
	}

	for (auto& item : m_OpaqueRitems) {
		item->ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device, /*1*/(UINT)m_OpaqueRitems.size(), true);

		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = item->ObjectCB->Resource()->GetGPUVirtualAddress();
		int boxCBufIndex = 0;
		cbAddress += boxCBufIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		m_Device->CreateConstantBufferView(&cbvDesc, m_CbvHeap2->GetCPUDescriptorHandleForHeapStart());
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
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig, &errorBlob);

	if (errorBlob != nullptr)
	{
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
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"..\\..\\..\\src\\EngineDev\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0").Get();
	m_psByteCode = d3dUtil::CompileShader(L"..\\..\\..\\src\\EngineDev\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0").Get();

	m_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void DirectXWindowManager::BuildBoxGeometry()
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
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

	m_BoxGeo = std::make_unique<MeshGeometry>();
	m_BoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, vertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);

	m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);

	m_BoxGeo->VertexByteStride = sizeof(Vertex);
	m_BoxGeo->VertexBufferByteSize = vbByteSize;
	m_BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_BoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_BoxGeo->DrawArgs["box"] = submesh;
}

void DirectXWindowManager::BuildPrismeTriangleGeo_btriangle() {
	std::array<Vertex, 4> vertices =
	{
		// Les 3 points de la base
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),  // 0
		Vertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) }),     // 1
		Vertex({ XMFLOAT3(0.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Green) }),    // 2
		// Le sommet de la pyramide
		Vertex({ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(Colors::Blue) })       // 3
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

	m_PrismeGeo = std::make_unique<MeshGeometry>();
	m_PrismeGeo->Name = "PrismeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_PrismeGeo->VertexBufferCPU));
	CopyMemory(m_PrismeGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_PrismeGeo->IndexBufferCPU));
	CopyMemory(m_PrismeGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_PrismeGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, vertices.data(), vbByteSize, m_PrismeGeo->VertexBufferUploader);

	m_PrismeGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, indices.data(), ibByteSize, m_PrismeGeo->IndexBufferUploader);

	m_PrismeGeo->VertexByteStride = sizeof(Vertex);
	m_PrismeGeo->VertexBufferByteSize = vbByteSize;
	m_PrismeGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_PrismeGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_PrismeGeo->DrawArgs["prisme_triangle"] = submesh;
}

void DirectXWindowManager::BuildPrismeTriangleGeo_bcarre()
{
	std::array<Vertex, 5> vertices =
	{
		// v1 up
		//Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}),  // 0
		//Vertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) }),     // 1
		////
		//Vertex({ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Magenta) }),    // 2
		//Vertex({ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Green) }),    // 3

		//////sommet
		//Vertex({ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(Colors::Blue) })       // 4


		// v2 right side // not complete // false coordinate for the moment
		//Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),  // 0
		//Vertex({ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(Colors::Red) }),     // 1

		//Vertex({ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Magenta) }),    // 2
		//Vertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),    // 3

		////sommet
		//Vertex({ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Blue) })       // 4


		// v3 back side
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),  // 0
		Vertex({ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Red) }),     // 1

		Vertex({ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Magenta) }),    // 2
		Vertex({ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),    // 3

		//sommet
		Vertex({ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Blue) })       // 4
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

	m_Prisme2Geo = std::make_unique<MeshGeometry>();
	m_Prisme2Geo->Name = "Prisme2Geo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_Prisme2Geo->VertexBufferCPU));
	CopyMemory(m_Prisme2Geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_Prisme2Geo->IndexBufferCPU));
	CopyMemory(m_Prisme2Geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_Prisme2Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, vertices.data(), vbByteSize, m_Prisme2Geo->VertexBufferUploader);

	m_Prisme2Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_Device,
		m_CommandList, indices.data(), ibByteSize, m_Prisme2Geo->IndexBufferUploader);

	m_Prisme2Geo->VertexByteStride = sizeof(Vertex);
	m_Prisme2Geo->VertexBufferByteSize = vbByteSize;
	m_Prisme2Geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_Prisme2Geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_Prisme2Geo->DrawArgs["prisme_triangle_carre"] = submesh;
}

//sphere

void DirectXWindowManager::SphereGeo()
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	int sliceCount = 0;
	int stackCount = 0;
	float radius = 1.0f;

	//sommet du haut (pôle nord)
	vertices.push_back({ XMFLOAT3(0.0f, radius, 0.0f) });

	float phiStep = XM_PI / stackCount;     // Angle anneau
	float thetaStep = 2.0f * XM_PI / sliceCount;  // Angle colonne

	// vertices anneaux
	for (int i = 1; i < stackCount; i++) {
		float phi = i * phiStep;

		for (int j = 0; j <= sliceCount; j++) {
			float theta = j * thetaStep;

			XMFLOAT3 pos = XMFLOAT3(
				radius * sinf(phi) * cosf(theta),
				radius * cosf(phi),
				radius * sinf(phi) * sinf(theta)
			);

			vertices.push_back({ pos });
		}
	}

	// sommet du bas (pôle sud)
	vertices.push_back({ XMFLOAT3(0.0f, -radius, 0.0f) });

	//  Création des indices pour les triangles
	//  Pôles premier anneau
	for (int i = 1; i <= sliceCount; i++)
		indices.push_back(0), indices.push_back(i + 1), indices.push_back(i);

	// Corps sphère
	int baseIndex = 1;
	int ringVertexCount = sliceCount + 1;
	for (int i = 0; i < stackCount - 2; i++) {
		for (int j = 0; j < sliceCount; j++) {
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// Dernier anneau vers le pôle sud
	int southPoleIndex = (int)vertices.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (int i = 0; i < sliceCount; i++) {
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}

//void DirectXWindowManager::BuildShadersAndInputLayout()
//{
//	m_vsByteCode["standardVS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
//	m_vsByteCode["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");
//
//	m_InputLayout =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	};
//}



void DirectXWindowManager::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
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

	//
	// PSO for opaque wireframe objects.
	//

	/*D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&m_PSO["opaque_wireframe"])));*/
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
	if (m_Fence->GetCompletedValue() < m_CurrentFence)
	{
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

	//m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	//assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	return true;
}

void DirectXWindowManager::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame. These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	// Compute averages over one second period.
	if ((m_TimerPtr->TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		std::wstring windowText = std::wstring(WINDOW_TITLE) +
			L" fps: " + fpsStr +
			L" mspf: " + mspfStr;

		SetWindowText(m_hMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void DirectXWindowManager::OnResize() {
	assert(m_Device);
	assert(m_SwapChain);
	assert(m_DirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		if (m_SwapChainBuffer[i])
		{
			m_SwapChainBuffer[i]->Release();
			m_SwapChainBuffer[i] = nullptr;
		}
		if (m_DepthStencilBuffer)
		{
			m_DepthStencilBuffer->Release();
			m_DepthStencilBuffer = nullptr;
		}
	}

	// Resize the swap chain.
	ThrowIfFailed(m_SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_CurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i], nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = WINDOW_WIDTH;
	depthStencilDesc.Height = WINDOW_HEIGHT;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
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
	ID3D12CommandList* cmdsLists[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(WINDOW_WIDTH);
	mScreenViewport.Height = static_cast<float>(WINDOW_HEIGHT);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	// Ajouter après la définition du scissor rect :
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, this->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

void DirectXWindowManager::OnMouseDown(WPARAM btnState, int x, int y) {
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
	SetCapture(m_hMainWnd);
}

void DirectXWindowManager::OnMouseUp(WPARAM btnState, int x, int y) {
	ReleaseCapture();
}

void DirectXWindowManager::OnMouseMove(WPARAM btnState, int x, int y) {
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta -= dx;
		m_Phi -= dy;

		// Restrict the angle phi.
		m_Phi = MathHelper::Clamp(m_Phi, 0.1f, XM_PI - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.05f * static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.05f * static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = MathHelper::Clamp(m_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

}

void DirectXWindowManager::BuildRenderItems()
{
	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(-8.0f, 0.0f, 0.0f)); // Centré
	XMStoreFloat4x4(&boxRitem->World, XMMatrixIdentity());
	boxRitem->ObjCBIndex = 0;
	boxRitem->ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device, 1, true);
	if (!boxRitem->ObjectCB || !boxRitem->ObjectCB->Resource()) {
		std::cerr << "[ERREUR] ObjectCB non initialisé dans BuildRenderItems() !" << std::endl;
	}
	else {
		std::cerr << "[DEBUG] ObjectCB bien créé pour ObjCBIndex = " << boxRitem->ObjCBIndex << std::endl;
	}

	if (m_BoxGeo) {
		boxRitem->Geo = m_BoxGeo.get();
	}
	else {
		std::cerr << "Erreur : m_BoxGeo est nullptr !" << std::endl;
	}
	//boxRitem->Geo = m_BoxGeo.get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	m_OpaqueRitems.push_back(std::move(boxRitem));

	auto pyramideRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pyramideRitem->World, XMMatrixTranslation(0.0f, 0.0f, 0.0f)); // Décalé à droite
	XMStoreFloat4x4(&pyramideRitem->World, XMMatrixIdentity());
	pyramideRitem->ObjCBIndex = 1;
	pyramideRitem->ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device, 1, true);


	if (m_Prisme2Geo) {
		pyramideRitem->Geo = m_Prisme2Geo.get();
	}
	else {
		std::cerr << "Erreur : m_Prisme2Geo est nullptr !" << std::endl;
	}
	//pyramideRitem->Geo = m_Prisme2Geo.get();
	pyramideRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pyramideRitem->IndexCount = pyramideRitem->Geo->DrawArgs["prisme_triangle_carre"].IndexCount;
	pyramideRitem->StartIndexLocation = pyramideRitem->Geo->DrawArgs["prisme_triangle_carre"].StartIndexLocation;
	pyramideRitem->BaseVertexLocation = pyramideRitem->Geo->DrawArgs["prisme_triangle_carre"].BaseVertexLocation;

	m_OpaqueRitems.push_back(std::move(pyramideRitem));


	// pyramide triangle
	auto pyramideTriRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pyramideTriRitem->World, XMMatrixTranslation(3.0f, 0.0f, 0.0f)); // Décalé à gauche
	//XMStoreFloat4x4(&pyramideTriRitem->World, XMMatrixIdentity());
	pyramideTriRitem->ObjCBIndex = 2;
	pyramideTriRitem->ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device, 1, true);


	if (m_PrismeGeo) {
		pyramideTriRitem->Geo = m_PrismeGeo.get();
	}
	else {
		std::cerr << "Erreur : m_PrismeGeo est nullptr !" << std::endl;
	}
	//pyramideRitem->Geo = m_Prisme2Geo.get();
	pyramideTriRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pyramideTriRitem->IndexCount = pyramideTriRitem->Geo->DrawArgs["prisme_triangle"].IndexCount;
	pyramideTriRitem->StartIndexLocation = pyramideTriRitem->Geo->DrawArgs["prisme_triangle"].StartIndexLocation;
	pyramideTriRitem->BaseVertexLocation = pyramideTriRitem->Geo->DrawArgs["prisme_triangle"].BaseVertexLocation;

	m_OpaqueRitems.push_back(std::move(pyramideTriRitem));

	//GeoSphere

		/*AddRenderItem("box", XMFLOAT3(0.0f, 0.0f, 0.0f));
	AddRenderItem("prisme_triangle", XMFLOAT3(2.0f, 0.0f, 0.0f));
	AddRenderItem("prisme_carre", XMFLOAT3(-2.0f, 0.0f, 0.0f));
	AddRenderItem("geosphere", XMFLOAT3(5.0f, 0.0f, 0.0f));*/
}

void DirectXWindowManager::AddRenderItem(const std::string& geoName, XMFLOAT3 position) {
	auto renderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&renderItem->World, XMMatrixTranslation(position.x, position.y, position.z));
	renderItem->ObjCBIndex = (UINT)m_OpaqueRitems.size();
	renderItem->Geo = m_Geometries[geoName].get();
	renderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderItem->IndexCount = renderItem->Geo->DrawArgs[geoName].IndexCount;
	renderItem->StartIndexLocation = renderItem->Geo->DrawArgs[geoName].StartIndexLocation;
	renderItem->BaseVertexLocation = renderItem->Geo->DrawArgs[geoName].BaseVertexLocation;
	m_OpaqueRitems.push_back(std::move(renderItem));
}