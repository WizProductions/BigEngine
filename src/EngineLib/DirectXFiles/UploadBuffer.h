#pragma once

template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
        : mDevice(device), mIsConstantBuffer(isConstantBuffer)
    {
        //UploadBuffer<T>* bufferPtr = new UploadBuffer<T>(device, elementCount, isConstantBuffer);
        mElementByteSize = sizeof(T);

        if (isConstantBuffer)
            mElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));
        else
            mElementByteSize = sizeof(T);

        auto HeapTypeUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto BufferElements = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);

        ThrowIfFailed(mDevice->CreateCommittedResource(
            &HeapTypeUpload,
            D3D12_HEAP_FLAG_NONE,
            &BufferElements,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)));

        ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
        if (!mMappedData)
        {
            throw std::runtime_error("Erreur : mUploadBuffer->Map() a �chou�, mMappedData est nullptr !");
        }
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

    ~UploadBuffer()
    {
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    ID3D12Resource* Resource() const
    {
        return mUploadBuffer.Get();
    }

    void CopyData(int elementIndex, const T& data)
    {
        if (!mMappedData)
        {
            throw std::runtime_error("Erreur : mMappedData est nullptr dans CopyData() !");
        }
        /*if (elementIndex < 0 || elementIndex >= mElementByteSize)
        {
            throw std::runtime_error("Erreur : elementIndex hors limites dans CopyData() !");
        }*/
        

        memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
    }

private:
    ID3D12Device* mDevice = nullptr;  //Ajout du pointeur sur device
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};
