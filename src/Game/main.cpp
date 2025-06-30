#include "pch.h"
#include "main.h"
#include "Game/Scenes/MainScene.h"
#include "Game/Scripts/SceneScriptTest.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd) {

	ApplicationManager* app = ApplicationManager::Get();
	app->Init();
	
	app->AddConsole();
	app->StartGame<MainScene, SceneScriptTest>();
	
	return 0;
}
