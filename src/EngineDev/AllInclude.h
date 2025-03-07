#pragma once


#include "iostream"
#include "ShlObj.h"
#include "random"
#include "map"
#include "array"
#include "comdef.h"
#include "fstream"
#include "windowsx.h"
#include "functional"
#include "chrono"

//Utils
#include "MiscUtils.h"

//Maths
#include "Transform.h"

//DirectX Additional files
#include "D3Dcompiler.h"
#include "DirectXColors.h"
#include "MathHelper.h"
#include "d3dUtil.h"
#include "d3dx12.h"
#include "GameTimer.h"
#include "UploadBuffer.h"

//Enumerations
#include "CollisionsEnum.h"
#include "SystemsEnum.h"

//Structs
#include "CollidingResult.h"

//Managers
#include "ApplicationManager.h"
#include "DirectXWindowManager.h"
#include "EventsManager.h"
#include "GameManager.h"
#include "InputsManager.h"
#include "LogsManager.h"
#include "SystemsManager.h"
#include "ThreadsManager.h"
#include "TimerManager.h"

//Scenes
#include "Scene.h"

//Systems
#include "ECSystem.h"
#include "CollisionsSystem.h"
#include "HealthSystem.h"
#include "PhysicsSystem.h"
#include "ScriptsSystem.h"

//Components
#include "Component.h"
#include "CollisionComponent.h"
#include "SphereCollisionComponent.h"
#include "AABBCollisionComponent.h"
#include "OBBCollisionComponent.h"
#include "HealthComponent.h"
#include "MeshComponent.h"
#include "PhysicsComponent.h"

//Scripts
#include "Script.h"
#include "SceneScript.h"
#include "EntityScript.h"

//Entities
#include "Entity.h"
