#pragma once

class DirectXWindowManager;

class ApplicationManager {
//##############################################################################
	//##------------------------------- ATTRIBUTES -------------------------------##
	//##############################################################################

private:
	/* FLAGS */
	bool m_Initialized;
	bool m_AppIsPaused;
	
    GameTimer m_Timer;
	
	DirectXWindowManager* m_DirectXWindowManager;

public:
	

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/**----------< CONSTRUCTORS >----------*/

	ApplicationManager() = default;
	~ApplicationManager() = default;

	/**------------------------------------*/
	

	/* GETTERS */

	static ApplicationManager* Get();
	GETTER bool& AppIsPaused() { return m_AppIsPaused; }
	GETTER GameTimer& GetTimer() { return m_Timer; }

/* SETTERS */




/* OTHERS FUNCTIONS */

	bool Init();
	void UnInit();

	template <typename S, typename Ss>
	void StartGame() const;
	void AddConsole();
};



#include "ApplicationManager.inl"