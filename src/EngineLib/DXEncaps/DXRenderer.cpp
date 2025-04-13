#include "pch.h"
#include "DXRenderer.h"

DXRenderer::DXRenderer() { this->UnInit(); }
DXRenderer::~DXRenderer() { DESTRUCTOR_UNINIT(m_Initialized); }

bool DXRenderer::Init() {

	if (m_Initialized)
		return false;

	return m_Initialized = true;
	
}

void DXRenderer::UnInit() {

	m_Initialized = false;
}

void DXRenderer::Update() {}
void DXRenderer::Draw() {}
void DXRenderer::OnResize() {}
