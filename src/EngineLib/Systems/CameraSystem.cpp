#include "pch.h"
#include "CameraSystem.h"

CameraSystem::CameraSystem() { this->UnInit(); }
CameraSystem::~CameraSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool CameraSystem::Init() {
    ECSystem::Init();
    
    if (m_Initialized)
        return false;

    m_SystemType = SystemType::Camera;
    m_SystemID = SystemId::Camera;
    
    m_Initialized = true;
    return true;
}

void CameraSystem::UnInit() {
    ECSystem::UnInit();

    UNINIT_PTR(m_SelectedCamera);
    m_Initialized = false;
}

void CameraSystem::Update() {

    if (m_SelectedCamera) { //STICK ON THE ENTITY
        if (m_SelectedCamera->m_bStickOnEntity) {

            const XMVECTOR entityPos = XMLoadFloat3(&m_SelectedCamera->m_AttachedEntity->m_Transform.GetPosition());
            const XMVECTOR vNewCameraPos = (entityPos - m_SelectedCamera->m_DistToEntity * m_SelectedCamera->m_AttachedEntity->m_Transform.GetForwardVector());

            XMFLOAT3 newLocalRotation;
            XMStoreFloat3(&newLocalRotation, m_SelectedCamera->m_AttachedEntity->m_Transform.GetLocalRotation());
            
            XMFLOAT3 newWorldRotation;
            XMStoreFloat3(&newWorldRotation, m_SelectedCamera->m_AttachedEntity->m_Transform.GetWorldRotation());
            
            m_SelectedCamera->m_Transform.SetLocalRotation(newLocalRotation.x, newLocalRotation.y, newLocalRotation.z);
            m_SelectedCamera->m_Transform.SetWorldRotation(newWorldRotation.x, newWorldRotation.y, newWorldRotation.z);

            XMFLOAT3 newCameraPos;
            XMStoreFloat3(&newCameraPos, vNewCameraPos);
            m_SelectedCamera->m_Transform.SetPosition(newCameraPos);
            m_SelectedCamera->m_Transform.UpdateTransformMatrix();
        }
    }
}

void CameraSystem::OnComponentAdded(Entity& entity) {
    ECSystem::OnComponentAdded(entity);

    //TODO: REINTERPRETED CAST TO COMPONENT PARAMETER? BETTER?

    CameraComponent* cameraC = SystemsManager::Get().GetComponentOfType<CameraComponent>(entity);
    cameraC->m_AttachedEntity = &entity;
}
void CameraSystem::HandleInput() {
    
    /*for (auto* camera : m_CameraComponents) {
        if (!camera) continue;

        DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };

        // Gestion du d�placement ZQSD
        if (GetAsyncKeyState('Z') & 0x8000) direction.z += 1.0f;
        if (GetAsyncKeyState('S') & 0x8000) direction.z -= 1.0f;
        if (GetAsyncKeyState('Q') & 0x8000) direction.x -= 1.0f;
        if (GetAsyncKeyState('D') & 0x8000) direction.x += 1.0f;

        // Application du mouvement
        if (direction.x != 0.0f || direction.z != 0.0f) {
            // Normalisation de la direction
            float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);
            if (length > 0.0f) {
                direction.x /= length;
                direction.z /= length;
            }

            // DirectX::XMFLOAT3 currentPos = camera->GetPosition();
            // currentPos.x += direction.x * camera->m_MovementSpeed;
            // currentPos.z += direction.z * camera->m_MovementSpeed;
            // camera->SetPosition(currentPos);
        }

        // Gestion de la rotation avec la souris
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT currentMousePos;
            GetCursorPos(&currentMousePos);

            // Au premier clic, initialiser la derni�re position
            if (camera->m_LastMousePos.x == 0.0f && camera->m_LastMousePos.y == 0.0f) {
                camera->m_LastMousePos = {
                    static_cast<float>(currentMousePos.x),
                    static_cast<float>(currentMousePos.y)
                };
                continue;
            }

            float deltaX = static_cast<float>(currentMousePos.x - camera->m_LastMousePos.x);
            float deltaY = static_cast<float>(currentMousePos.y - camera->m_LastMousePos.y);

            // // Mise � jour des angles
            // camera->m_CurrentYaw += deltaX * camera->m_RotationSpeed;
            // camera->m_CurrentPitch += deltaY * camera->m_RotationSpeed;
            //
            // // Limiter la rotation verticale entre -89� et 89�
            // //camera->m_CurrentPitch = std::clamp(camera->m_CurrentPitch, -89.0f, 89.0f);
            // // Appliquer la rotation
            // camera->m_Transform.SetWorldRotation(camera->m_CurrentPitch, camera->m_CurrentYaw, 0.0f);
            //
            // // Sauvegarder la position actuelle de la souris
            // camera->m_LastMousePos = {
            //     static_cast<float>(currentMousePos.x),
            //     static_cast<float>(currentMousePos.y)
            // };
        }
        else {
            // R�initialiser la derni�re position de la souris quand le bouton est rel�ch�
            camera->m_LastMousePos = { 0.0f, 0.0f };
        }
    }*/
}

CameraSystem& CameraSystem::Get() {
    static CameraSystem instance;
    return instance;
}
