//
// C++ Interface: ShaderManager
//
// Description:
//
//
// Author: Alexey Torkhov <atorkhov@gmail.com>, (C) 2009
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.//
//
#ifndef EMBEROGRE_SHADERMANAGER_H
#define EMBEROGRE_SHADERMANAGER_H

#include "framework/ConsoleObject.h"
#include "services/config/ConfigListenerContainer.h"
#include "OgreIncludes.h"

#include <string>
#include <map>
#include <sigc++/signal.h>

namespace Ember {
namespace OgreView {

class ShadowCameraSetup;
class ShaderSetupInstance;
class IGraphicalChangeAdapter;
class ShaderDetailManager;

/**
 * @brief Utility class for setup shaders
 *
 * Utility class for setup shaders. It checks a set of materials and selects
 * best sheme they work at. Then it performs setup of auxilary things like
 * shadows.
 *
 * @author Alexey Torkhov <atorkhov@gmail.com>
 */
class ShaderManager : public ConsoleObject, public ConfigListenerContainer
{
public:
	/**
	 * Enumeration of graphics levels
	 */
	enum GraphicsLevel {
		// Default scheme is when no scheme specified, very simple textured models
		LEVEL_DEFAULT,

		// Low level, shader model 2
		LEVEL_LOW,

		// Medium level, shader model 3
		LEVEL_MEDIUM,

		// High level, shader model 4
		LEVEL_HIGH,

		// Experimental level
		LEVEL_EXPERIMENTAL,
	};

	/**
	 * Constructor.
	 */
	ShaderManager(IGraphicalChangeAdapter& graphicalChangeAdapter);

	/**
	 * Destructor.
	 */
	~ShaderManager();

	/**
	 * Shaders initialization
	 */
	void init();

	/**
	 * Gets current graphics level
	 */
	GraphicsLevel getGraphicsLevel();

	/**
	 * Sets current graphics level
	 */
	GraphicsLevel setGraphicsLevel(GraphicsLevel newLevel);

	/**
	 * Reimplements the ConsoleObject::runCommand method
	 */
	virtual	void runCommand(const std::string &command, const std::string &args);

	/**
	 * Sets graphics level at runtime
	 */
	const ConsoleCommandWrapper SetLevel;

	/**
	 * Returns level id by its name
	 */
	GraphicsLevel getLevelByName(const std::string &level);
	
	/**
	 * @brief Returns a map of levels and their scheme names.
	 * Can be used to retrieve all schemes as known to the shader manager.
	 */
	const std::map<GraphicsLevel, std::string>& getGraphicsScheme() const;

	/**
	 * @brief Registers a scene manager with the shader manager.
	 *
	 * This allows the shader manager to handle the shadow and shader setup for the scene manager.
	 * @param sceneManager The scene manager to register.
	 */
	void registerSceneManager(Ogre::SceneManager* sceneManager);

	/**
	 * @brief Deregisters a scene manager with the shader manager.
	 *
	 * @param sceneManager The scene manager to deregister.
	 */
	void deregisterSceneManager(Ogre::SceneManager* sceneManager);

	/**
	 * @brief Emitted when the graphical level is changed.
	 */
	sigc::signal<void> EventLevelChanged;

private:

	typedef std::map<Ogre::SceneManager*, ShaderSetupInstance*> ShaderSetupStore;
	/**
	 * Current graphics level
	 */
	GraphicsLevel mGraphicsLevel;

	/**
	 * Best graphics level supported, determined at initialization time
	 */
	GraphicsLevel mBestGraphicsLevel;

	/**
	 * Map of levels and schemes. Also used to convert levels to strings
	 */
	std::map<GraphicsLevel, std::string> mGraphicSchemes;

	ShaderSetupStore mShaderSetups;
	
	/**
	 * Object that handles management of shader levels automatically.
	 */
	ShaderDetailManager* mShaderDetailManager;

	/**
	 * Checks whether material is supported in current scheme
	 */
	bool checkMaterial(const std::string& materialName, const std::string& schemeName);

	/**
	 * Setups PSSM shadows
	 */
	void setPSSMShadows();

	/**
	 * Disables shadows
	 */
	void setNoShadows();

	/**
	 * @brief Called when the varconf setting for the graphics level changes. This will in turn call setGraphicsLevel.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_Level(const std::string& section, const std::string& key, varconf::Variable& variable);
	
	/**
	 * Reference to the graphical change adapter through which graphics detail changes are requested that is to be passed on when some member objects are created outside of the constructor.
	 */
	IGraphicalChangeAdapter& mGraphicalChangeAdapter;

};

}

}

#endif
