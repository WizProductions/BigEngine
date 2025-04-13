#include "pch.h"
#include "GameManager.h"

GameManager::GameManager() { this->UnInit();  }
GameManager::~GameManager() { DESTRUCTOR_UNINIT(m_Initialized); }

bool GameManager::Init() {

	if (m_Initialized)
		return false;

	//INIT_PTR(m_RenderWindow, ApplicationManager::Get()->GetRenderWindowRef());

	ApplicationManager& app = *ApplicationManager::Get();
    INIT_PTR(m_TimerPtr, &app.GetTimer());
    INIT_PTR(m_AppIsPausedPtr, &app.AppIsPaused());

	INIT_NEW_PTR(m_Entities, Entity*[entityArraySize](nullptr)); //Above SystemManager init
	INIT_PTR(m_SystemsManager, &SystemsManager::Get());
	INIT_PTR(m_InputsManager, &Wiz::InputsManager::Get());
	//DEFAULT CAMERA IS NULLPTR & DIRECTX USE IDENTITY MATRIX

	CameraComponent* cameraC = new CameraComponent;
	cameraC->m_Transform.SetPosition(0, 0, -10);

	m_SystemsManager->Init();
	m_InputsManager->Init();
	
	m_Timer = GameTimer();

	m_Initialized = true;
	return true;
}

void GameManager::UnInit() {
	
	UNINIT_PTR(m_Scene);
	UNINIT_PTR(m_Entities);
	
	m_EntitiesPendingSpawn.clear();
	m_EntitiesPendingDestroy.clear();

	m_AppIsPausedPtr = nullptr;
	
	m_EntityArrayNeedToBeRealloc = false;
	m_Initialized = false;
}


GameManager* GameManager::Get() {
	static GameManager mInstance;
	return &mInstance;
}


void GameManager::StartLoop() {

    MSG msg = {nullptr};
    m_Timer.Reset();

	m_SystemsManager->Start(m_Entities);
	
	while(msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        while(PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        
		m_Timer.Tick();
		*m_AppIsPausedPtr = false;
		
		if( !*m_AppIsPausedPtr )
		{
			this->Update();
			this->Draw();
		}
		else
		{
			Sleep(100);
		}
    }
}

void GameManager::Update() {
	// New entities
	for (const auto entityToSpawn : m_EntitiesPendingSpawn) {
		lastEntityIndex++;

		if (m_EntityArrayNeedToBeRealloc) {
			entityArraySize <<= 1;
			assert(realloc(m_Entities, entityArraySize * sizeof(Entity*)) != nullptr);
			m_EntityArrayNeedToBeRealloc = false;
		}

		m_Entities[lastEntityIndex] = entityToSpawn;
		m_SystemsManager->Start(*entityToSpawn);
	}
	m_EntitiesPendingSpawn.clear();

	// Old Entities
	for (const auto entityToDestroy : m_EntitiesPendingDestroy) {
		const int entityToDestroyIndex = entityToDestroy->m_SharedIndex;

		m_SystemsManager->RemoveAllComponents(*entityToDestroy);
		delete entityToDestroy;

		if (entityToDestroyIndex == lastEntityIndex) {
			m_Entities[lastEntityIndex] = nullptr;
			lastEntityIndex--;
			continue;
		}

		m_Entities[entityToDestroyIndex] = m_Entities[lastEntityIndex];
		m_Entities[entityToDestroyIndex]->m_SharedIndex = entityToDestroyIndex;
		m_SystemsManager->SwapComponentsList(lastEntityIndex, entityToDestroyIndex);

		m_Entities[lastEntityIndex] = nullptr;
		lastEntityIndex--;
	}
	m_EntitiesPendingDestroy.clear();

	m_InputsManager->CaptureAllKeys();
	m_SystemsManager->Update();
	DirectXWindowManager::Get().Update();
}

void GameManager::Draw() {

	/*m_RenderWindow->clear(sf::Color(100, 100, 100)); //Grey background

	for (const auto gameObject : m_GameObjects) {
		gameObject->OnDraw(*m_RenderWindow);
	}

	m_RenderWindow->display();*/
	
	DirectXWindowManager::Get().Draw();
}

void GameManager::HandleInput() {
	/*sf::Event event;
    while (m_RenderWindow->pollEvent(event)) {
    	if (event.type == sf::Event::Closed)
        {
            m_RenderWindow->close();
        }

    	m_Scene->OnEvent(event);
    }*/
}

void GameManager::AddEntity(Entity* entity) {
	entity->m_SharedIndex = lastEntityIndex + 1 + m_EntitiesPendingSpawn.size();

	if (lastEntityIndex + 1 >= entityArraySize - 1) {
		const int newArraySize = entityArraySize * 2;
		assert(newArraySize <= INT_MAX);

		assert(m_SystemsManager->ResizeComponentsArray(newArraySize * sizeof(Entity*)) != nullptr);
		m_EntityArrayNeedToBeRealloc = true;
	}

	m_EntitiesPendingSpawn.push_back(entity);
}

void GameManager::RemoveEntity(Entity* entity) {
	m_EntitiesPendingDestroy.push_back(entity);
}

void GameManager::ResetEntities() {
	
	m_EntitiesPendingSpawn.clear();

	for (int i(0); i <= lastEntityIndex; i++) {
		const int entityIndex = m_Entities[i]->m_SharedIndex;
		delete m_Entities[i];
		m_Entities[entityIndex] = nullptr;
	}
	
	m_EntitiesPendingDestroy.clear();
}



void GameManager::AsyncExecutePacketInstructions() {

	/*Wiz::Packet* packet = static_cast<Wiz::Packet*>(threadStruct->m_Param);
	
	switch (packet->GetPacketHeader().m_MessageType) {
	case Wiz::Packet::MessageType::CONNECTING:
		{
			std::cout << "GameManager(): [RECEIVE] CONNECTING" << std::endl;
			NetworkingComponentClient client;
			client.Init();
			client.Setup("127.0.0.1", static_cast<SOCKADDR_IN*>(threadStruct->m_Param2)->sin_port);

			Wiz::Packet responsePacket;
			responsePacket.Init(1, Wiz::Packet::MessageType::CONNECTING_RESPONSE, "connected");
			client.SendMessage(responsePacket);
		
			delete static_cast<SOCKADDR_IN*>(threadStruct->m_Param2);
		
			break;
		}
	
	default:
		std::cerr << "GameManager(): [RECEIVE] UNKNOWN MESSAGE" << std::endl;
		break;
	}*/
}
