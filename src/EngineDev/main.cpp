#include "pch.h"
#include "main.h"
#include "Managers/ApplicationManager.h"
#include "Scenes/MainScene.h"
#include "Scripts/SceneScriptTest.h"
#include "Entities/Entity.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow) {

	ApplicationManager* app = ApplicationManager::Get();
	app->Init();
	
	app->AddConsole();
 	app->StartGame<MainScene, SceneScriptTest>();

	
    /*class Control
    {
    public:
    	
    	void Show(int flag)
    	{
    		m_visible = true;
    		onShow.Call(flag);
    	}

    	
    	EventsManager<int> onShow;
    	EventsManager<int, float> onHide;

    	
    private:
    	bool m_visible = false;
    };
    
    class MyClass
    {
    public:
    	void OnShow(int flag)
    	{
        }
    	void OnHide(int x, float time)
    	{
        }
    	void Register(Control* pCtrl)
    	{
    		pCtrl->onShow.Register(this, &MyClass::OnShow);
    		pCtrl->onHide.Register(this, &MyClass::OnHide);
    	}
    };
    
    Control ctrl;
    MyClass my;
    my.Register(&ctrl);
    ctrl.Show(182); #2#*/

	return 0;
}