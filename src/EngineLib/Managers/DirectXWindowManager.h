#pragma once

struct WindowInformation;
class GameTimer;
struct IDXGIFactory4;
struct IDXGISwapChain;

/*
 *	https://github.com/yottaawesome/intro-to-dx12 -> It's the base of this class.
 */

class DirectXWindowManager {

	//##############################################################################
	//##------------------------------- ATTRIBUTES -------------------------------##
	//##############################################################################

	/* FLAGS */
	bool m_Initialized;
	bool m_lockMouseInWindow = false;

	HINSTANCE m_hAppInst = nullptr; // application instance handle
	DXRenderer* m_DXRenderer = nullptr;

	const int gNumFrameResources = 1;
	
	IDXGIFactory4* m_Factory;
	inline static ID3D12Device* m_Device;
	ID3D12Fence* m_Fence;
	UINT64 m_CurrentFence = 0;
	int m_CurrBackBuffer = 0;
	IDXGISwapChain* m_SwapChain;
	static constexpr int SwapChainBufferCount = 2;
	ID3D12Resource* m_SwapChainBuffer[SwapChainBufferCount];
	ID3D12Resource* m_DepthStencilBuffer;
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;
	ID3D12CommandQueue* m_CommandQueue;
	ID3D12CommandAllocator* m_DirectCmdListAlloc;
	inline static ID3D12GraphicsCommandList* m_CommandList;

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	ID3D12DescriptorHeap* m_RtvHeap;
	ID3D12DescriptorHeap* m_DsvHeap;

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	struct ObjectConstants {
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	std::unordered_map<std::string, MeshGeometry*> m_Geometries;

	ID3DBlob* m_vsByteCode = nullptr;
	ID3DBlob* m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
	
	ID3D12Resource* m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

	// Pipeline State Object et signature racine.
	ID3D12PipelineState* m_PSO = nullptr;
	ID3D12RootSignature* m_RootSignature;
	ID3D12DescriptorHeap* m_CbvHeap = nullptr;

	XMFLOAT4X4 m_View = MathHelper::Identity4x4();
	XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();

	MeshGeometry* m_pBoxGeo = nullptr;
	MeshGeometry* m_pPyramidTriangleGeo = nullptr;
	MeshGeometry* m_pPyramidSquaredGeo = nullptr;
	MeshGeometry* m_pSphereGeo = nullptr;

	POINT m_LastMousePos;

	struct RenderItem {
		XMFLOAT4X4* pEntityWorldMatrix;
		UINT ObjCBIndex = -1;
		UINT IndexCount = 0;
		MeshGeometry* Geo = nullptr;
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
		
		UploadBuffer<ObjectConstants>* ObjectCB = nullptr;
	};
	
	std::vector<RenderItem*> m_OpaqueRitems;

public:

	inline static WindowInformation* m_WindowInformationPtr = nullptr;
	UINT mCurrFrameResourceIndex = 0;

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/

	DirectXWindowManager();
	~DirectXWindowManager();

	/*------------------------------------*/

	bool Init(UINT16 windowWidth, UINT16 windowHeight, LPCWSTR windowTitle);
	void UnInit();

	/* GETTERS */

	_NODISCARD static DirectXWindowManager& Get();
	GETTER float GetAspectRatio() const { return m_WindowInformationPtr->aspectRatio; }
	GETTER D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() { return m_DsvHeap->GetCPUDescriptorHandleForHeapStart(); }
	GETTER ID3D12Resource* CurrentBackBuffer() { return m_SwapChainBuffer[m_CurrBackBuffer]; }
	GETTER bool GetLockMouseInWindow() const { return m_lockMouseInWindow; }
	GETTER const WindowInformation& GetWindowInformation() const { return *m_WindowInformationPtr; }

	/* SETTERS */

	/** Lock and hide/show the mouse pointer */
	SETTER void SetLockMouseInWindow(const bool newState) {
		m_lockMouseInWindow = newState;
		ShowCursor(!newState);
	}


	/* OTHERS FUNCTIONS */

	void Update();
	void Draw();
	bool InitializeWindow(UINT16 windowWidth, UINT16 windowHeight, LPCWSTR windowTitle);
	int InitializeDirectX3D();
	bool CheckMSAASupport();
	void CreateDevice(D3D_FEATURE_LEVEL minFeatureLevel, ID3D12Device*& device);
	void CreateCommandObjects();
	void CreateFence();
	void CreateSwapChain();
	void CreateRtvAndDescriptorsHeaps();
	void CreateRenderTargetView();
	void CreateCommittedResource();
	void FlushCommandQueue();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();
	void InitializePyramidTriangleGeo();
	void InitializePyramidSquaredGeo();
	void OnResize();
	void DrawEntity(const GeometryType& geoType, const Entity& entity);

	friend DXGeometry;

};