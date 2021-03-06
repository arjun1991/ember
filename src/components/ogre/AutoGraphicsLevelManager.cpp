/*
 * Copyright (C) 2012 Arjun Kumar <arjun1991@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <components/ogre/AutoGraphicsLevelManager.h>

#include <components/ogre/EmberOgre.h>

#include <services/config/ConfigListenerContainer.h>

#include <framework/MainLoopController.h>
#include <framework/Time.h>
#include <framework/LoggingInstance.h>

#include <Ogre.h>

#include <cmath>

namespace Ember
{
namespace OgreView
{

FpsUpdater::FpsUpdater(Ogre::RenderWindow& renderWindow) :
		mCurrentFps(0.0f), mTimeAtLastUpdate(Time::currentTimeMillis()), mTimeAtLastStore(Time::currentTimeMillis()), mRenderWindow(renderWindow), mTimeBetweenUpdates(2000), mRequiredTime(6.0f)
{
	Ogre::Root::getSingleton().addFrameListener(this);
}

FpsUpdater::~FpsUpdater()
{
	Ogre::Root::getSingleton().removeFrameListener(this);
}

bool FpsUpdater::frameStarted(const Ogre::FrameEvent& event)
{
	long currentTime = Time::currentTimeMillis();

	if (currentTime - mTimeAtLastStore >= 1000) {
		mTimeAtLastStore = currentTime;

		//Push back latest fps
		mFpsStore.push_back(mRenderWindow.getLastFPS());
		S_LOG_VERBOSE(mRenderWindow.getLastFPS() << " is pushed onto FPS queue");

		//if there is one more than the minimum number of fpses then delete oldest fps. This also ensures that the store fills up with at least mRequiredTime fpses before starting to remove fpses.
		if (mFpsStore.size() > mRequiredTime) { //Fps is measured every second, therefore when size > required time, oldest fps must be popped
			mFpsStore.pop_front();
		}
	}

	if (currentTime - mTimeAtLastUpdate >= mTimeBetweenUpdates) {
		mTimeAtLastUpdate = currentTime;

		float totalFps;
		int fpsCount = 0;
		for (std::deque<float>::iterator I = mFpsStore.begin(); I != mFpsStore.end(); ++I) {
			totalFps += *I;
			++fpsCount;
		}
		mCurrentFps = totalFps / fpsCount;
		
		fpsUpdated.emit(mCurrentFps);
		S_LOG_VERBOSE(mCurrentFps << " is emitted as current average FPS");
	}

	return true;
}

float FpsUpdater::getCurrentFPS()
{
	return mCurrentFps;
}

IGraphicalChangeAdapter::IGraphicalChangeAdapter()
{
}

IGraphicalChangeAdapter::~IGraphicalChangeAdapter()
{
}

bool IGraphicalChangeAdapter::fpsChangeRequired(float changeSize)
{
	//for now leaving it at this, need to update later with better callibrated values
	float translatedChangeRequired = changeSize / 1.0f;

	bool furtherChangePossible = changeRequired.emit(translatedChangeRequired);
	return furtherChangePossible;
}

AutomaticGraphicsLevelManager::AutomaticGraphicsLevelManager(Ogre::RenderWindow& renderWindow, MainLoopController& mainLoopController) :
		mEnabled(false), mDefaultFps(60), mFpsUpdater(renderWindow), mMainLoopController(mainLoopController), mConfigListenerContainer(new ConfigListenerContainer())
{
	mFpsUpdatedConnection = mFpsUpdater.fpsUpdated.connect(sigc::mem_fun(*this, &AutomaticGraphicsLevelManager::checkFps));
	mConfigListenerContainer->registerConfigListener("general", "desiredfps", sigc::mem_fun(*this, &AutomaticGraphicsLevelManager::Config_DefaultFps));
	mConfigListenerContainer->registerConfigListenerWithDefaults("graphics", "autoadjust", sigc::mem_fun(*this, &AutomaticGraphicsLevelManager::Config_Enabled), false);
	S_LOG_INFO("Automatic graphics detail manager is initialized");
}

AutomaticGraphicsLevelManager::~AutomaticGraphicsLevelManager()
{
	delete mConfigListenerContainer;
}

void AutomaticGraphicsLevelManager::setFps(float fps)
{
	mDefaultFps = fps;
}

void AutomaticGraphicsLevelManager::checkFps(float currentFps)
{
	float changeRequired = mDefaultFps - currentFps;
	bool frameLimited = mMainLoopController.getFrameLimited();
	//This factor is used to adjust the required fps difference before a change is triggered. Lower required fpses eg. 30 will need to respond to smaller changes.
	float factor = mDefaultFps / 60.0f;
	if (std::abs(changeRequired) >= factor * 8.0f) {
		changeGraphicsLevel(changeRequired);
		S_LOG_VERBOSE("Fps difference of " << changeRequired << " detected, requesting detail change.");
	} else if (changeRequired < 2.0f && frameLimited) { //average fps is exactly equal to desired fps, fps is being limited.
		changeGraphicsLevel(-8.0f); //try to up the detail so fps drops by around 8 fps
		S_LOG_VERBOSE("Frame limiting detected" << "at fps " << currentFps << ", raising graphics detail.");
	}
}

void AutomaticGraphicsLevelManager::changeGraphicsLevel(float changeInFpsRequired)
{
	//TODO: Need to implement functionality for if further change is not possible
	mGraphicalChangeAdapter.fpsChangeRequired(changeInFpsRequired);
}

IGraphicalChangeAdapter& AutomaticGraphicsLevelManager::getGraphicalAdapter()
{
	return mGraphicalChangeAdapter;
}

void AutomaticGraphicsLevelManager::setEnabled(bool newEnabled)
{
	mEnabled = newEnabled;
	if (newEnabled == false) {
		mFpsUpdatedConnection.block();
	} else {
		mFpsUpdatedConnection.unblock();
	}
}

bool AutomaticGraphicsLevelManager::isEnabled()
{
	return mEnabled;
}

void AutomaticGraphicsLevelManager::Config_DefaultFps(const std::string& section, const std::string& key, varconf::Variable& variable)
{
	if (variable.is_double()) {
		float fps = static_cast<double>(variable);
		//If set to 0, the fps the manager tries to achieve is 60
		if (fps == 0.0f) {
			fps = 60.0f;
		}
		setFps(fps);
	}
}

void AutomaticGraphicsLevelManager::Config_Enabled(const std::string& section, const std::string& key, varconf::Variable& variable)
{
	if (variable.is_bool() && static_cast<bool>(variable)) {
		setEnabled(true);
	} else {
		setEnabled(false);
	}
}

}
}
