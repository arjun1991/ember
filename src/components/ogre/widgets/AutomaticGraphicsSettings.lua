AutomaticGraphicsSettings = {}

function AutomaticGraphicsSettings:buildWidget()

	self.widget = guiManager:createWidget()
	self.widget:loadMainSheet("AutomaticGraphicsSettings.layout","AutomaticGraphicsSettings/")
	
	self.window = self.widget:getWindow("MainFrame")
	self.widget:registerConsoleVisibilityToggleCommand("autosettings")

	local mainLayoutWindow = CEGUI.WindowManager:getSingleton():createWindow("VerticalLayoutContainer")
	
	local lineLayoutWindow = CEGUI.WindowManager:getSingleton():createWindow("HorizontalLayoutContainer")
	
	local label = CEGUI.WindowManager:getSingleton():createWindow("EmberLook/StaticText")
	label:setText("Foliage density factor: ")
	label:setProperty("UnifiedSize", "{{0.3, 0.0}, {0.0, 30.0}}")

	lineLayoutWindow:addChildWindow(label)
	
	local slider = CEGUI.WindowManager:getSingleton():createWindow("EmberLook/Slider")
	slider:setClickStep("0.05")
	
	lineLayoutWindow:addChildWindow(slider)
	
	mainLayoutWindow:addChildWindow(lineLayoutWindow)
	self.window:addChildWindow(mainLayoutWindow)
end

function AutomaticGraphicsSettings:shutdown()
	guiManager:destroyWidget(self.widget)
end

AutomaticGraphicsSettings.worldCreatedConnector = createConnector(emberOgre.EventWorldCreated):connect(function(world)
	
	local automaticGraphicsSettings = {connectors={}}
	setmetatable(automaticGraphicsSettings, {__index = AutomaticGraphicsSettings})
	automaticGraphicsSettings:buildWidget()
	
	-- must add packages for Foliage and FoliageLevelManager
	-- local foliageLevelManager = World:getFoliage():getFoliageLevelManager()

	connect(automaticGraphicsSettings.connectors, emberOgre.EventWorldBeingDestroyed, function()
		automaticGraphicsSettings:shutdown()
		automaticGraphicsSettings = nil
	end)
end)
	
	
