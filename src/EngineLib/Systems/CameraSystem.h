#pragma once

#include "ECSystem.h"
#include "Components/CameraComponent.h"

class CameraSystem final : public ECSystem {
private:
    bool m_Initialized;
    CameraComponent* m_SelectedCamera;

public:
    CameraSystem();
    ~CameraSystem() override;

    bool Init() override;
    void UnInit() override;
    void Update() override;

    static CameraSystem& Get();

    //Getters
    GETTER CameraComponent* GetSelectedCamera() const { return m_SelectedCamera; }

    //Setters
    SETTER void SetSelectedCamera(CameraComponent* camera) { m_SelectedCamera = camera; }

    void OnComponentAdded(Entity& entity) override;

private:
    void HandleInput();
};
