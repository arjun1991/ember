AutomaticGraphicsSettings = {}

function AutomaticGraphicsSettings:buildWidget()

	self.widget = guiManager:createWidget()
	self.widget:loadMainSheet("AutomaticGraphicsSettings.layout","AutomaticGraphicsSettings/")
	
	self.window = self.widget:getWindow("MainFrame")
	self.widget:registerConsoleVisibilityToggleCommand("autosettings")
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
	
	
