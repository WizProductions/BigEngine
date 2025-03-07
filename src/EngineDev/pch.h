#pragma once

#define Engine_

#include <iostream>
#include <ShlObj.h>
#include <random>
#include <map>
#include <array>
#include <comdef.h>
#include <fstream>
#include <windowsx.h>
#include <functional>
#include <chrono>

//Utils
#include <Utils/MiscUtils.h>

//Maths
#include <Maths/Transform.h>

//DirectX Additional files
#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <DirectXFiles/MathHelper.h>
#include <DirectXFiles/d3dUtil.h>
#include <DirectXFiles/d3dx12.h>
#include <DirectXFiles/GameTimer.h>
#include <DirectXFiles/UploadBuffer.h>

//Enumerations
#include <Enumerations/CollisionsEnum.h>
#include <Enumerations/SystemsEnum.h>

//Structs
#include <Structs/CollidingResult.h>

//Managers
#include <Managers/ApplicationManager.h>
#include <Managers/DirectXWindowManager.h>
#include <Managers/EventsManager.h>
#include <Managers/GameManager.h>
#include <Managers/InputsManager.h>
#include <Managers/LogsManager.h>
#include <Managers/SystemsManager.h>
#include <Managers/ThreadsManager.h>
#include <Managers/TimerManager.h>

//Scenes
#include <Scenes/Scene.h>

//Systems
#include <Systems/ECSystem.h>
//#include <Systems/CollisionsSystem.h>
#include <Systems/HealthSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/ScriptsSystem.h>

//Components
#include <Components/Component.h>
#include <Components/CollisionComponent.h>
#include <Components/SphereCollisionComponent.h>
#include <Components/AABBCollisionComponent.h>
#include <Components/OBBCollisionComponent.h>
#include <Components/HealthComponent.h>
#include <Components/MeshComponent.h>
#include <Components/PhysicsComponent.h>

//Scripts
#include <Scripts/Script.h>
#include <Scripts/SceneScript.h>
#include <Scripts/EntityScript.h>

//Entities
#include <Entities/Entity.h>

// #ifdef _DEBUG
// #include <crtdbg.h>
// #define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// #define new DEBUG_NEW
// #endif