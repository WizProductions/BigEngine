#include "pch.h"
#include "SceneScriptTest.h"

void SceneScriptTest::Start() {}

void SceneScriptTest::Update() {
	if (Wiz::InputsManager::Get().GetKeyState(Wiz::Key::ESCAPE) == Wiz::KeyState::PRESSED)
		DirectXWindowManager::Get().SetLockMouseInWindow(!DirectXWindowManager::Get().GetLockMouseInWindow());
}

void SceneScriptTest::End() {}