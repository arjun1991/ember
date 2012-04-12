#include <CEGUIWindowManager.h>
#include <CEGUIUDim.h>
#include <CEGUIWindow.h>
#include <elements/CEGUIFrameWindow.h>
#include <sstream>
#include "services/config/ConfigListenerContainer.h"



namespace Ember{
	
class FpsTracker: public ConfigListenerContainer, public Singleton<FpsTracker>
{
	
protected:
	/**
	 * @brief Holds the current FPS as updated from mainLoopStep from the application class.
	 */
	double mCurrentFps;
	
	/**
	 * @brief Boolean value that indicated wether the FPS window is visible or not.
	 */
	bool fpsVisible;
	
	/**
	 * @brief Holds a reference to the FPS window controlled by this class.
	 */
	CEGUI::FrameWindow * fpsDialog;
	
	
	/**
	 * @brief Pointer that holds a reference to the main sheet returned from the GUIManager.
	 */
	CEGUI::Window* mSheet;
	
	/**
	 * @brief Pointer that holds a reference to the main Window Manger.
	 */
	CEGUI::WindowManager* mWindowManager;
	
	/**
	 * @brief This is assigned with the scheme used to display the FPS frame window.
	 */
	std::string defaultScheme;
	
	long int currentTimeMillis;
	
	/**
	 * @brief Time as returned by Time::currentTimeMillis() when the FPS window was last updated with the current FPS. 
	 * Used to implement FPS update delay.
	 */ 
	long int timeAtLastUpdate;
	
public:
	FpsTracker();
	
	FpsTracker(double fps);
	
	/**
	 * @brief This function is called when FPS is enabled/disabled from the Settings Menu
	 */
	void Config_Fps_Visibility(const std::string& section, const std::string& key, varconf::Variable& variable);
	
	/**
	 * @brief This is used to update the FPS from mainLoopStep of Application class.
	 */
	void setCurrentFps(double fps);
    
	/**
	 * @brief Returns the current FPS value.
	 */
	double getCurrentFps();
    
	/**
	 * @brief Contains the implementation to optimize graphics properties based on FPS.
	 */
	void checkFpsOptimum();
    
};

}