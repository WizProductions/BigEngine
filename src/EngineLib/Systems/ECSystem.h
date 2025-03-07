#pragma once

//Forward declarations
struct Component;
struct Entity;
enum class SystemType : uint32_t;
//----

class ECSystem {

private:
    /* FLAGS */
    bool m_Initialized;

protected:
    SystemType m_SystemType;
    SystemId m_SystemID;
    Entity** m_Entities;
    std::list<Component*>** m_Components;
    
public:
    ECSystem();
    virtual ~ECSystem();

    GETTER SystemType GetSystemType() const { return m_SystemType; }
    GETTER SystemId GetSystemID() const { return m_SystemID; }

    virtual bool Init();
    virtual void UnInit();

    virtual void Start(Entity& entity);
    virtual void Start(Entity** entities, std::list<Component*>** components);
    virtual void Update() = 0;

    /* EVENTS */
    virtual void OnComponentAdded(Entity& entity);
};