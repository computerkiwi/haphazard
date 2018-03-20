--[[
FILE: PauseMenu.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- Keep track of active menu items and which one is selected.
local menuItems = {}
local itemSelected = 1

local pauseBackground

-- Main pause menu buttons.
local resumeButton
local restartButton
local settingsButton
local quitButton

-- Settings pause menu buttons.
local muteMusicButton
local muteSfxButton
local toggleFullscreenButton
local backButton

local inSettings = false

function SpawnAndAttachObject(prefabName, resetPosition)
	resetPosition = resetPosition or false

	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = this
		if (resetPosition)
		then
			obj:GetTransform().localPosition = vec2(0,0)
		end
	
		return obj
	else
		return nil
	end
end

function Start()
	pauseBackground = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/PauseBackground.json", true)
	
	-- TODO: Get rid of this by using real implementations on buttons.
	local function DummyFunc()
		print("Menu button not yet implemented!")
	end
	
	resumeButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	restartButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	settingsButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	quitButton = NewButton("treeboy.png", "ground.png", DummyFunc)

	-- TODO: Remove this when properly handling multiple states.
	menuItems = {resumeButton, restartButton, settingsButton, quitButton}
	
	-- Settings pause menu buttons.
	muteMusicButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	muteSfxButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	toggleFullscreenButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	backButton = NewButton("treeboy.png", "ground.png", DummyFunc)
	
	DeactivateAllButtons()
	
end

function DeactivateAllButtons()
	resumeButton:Deactivate()
	restartButton:Deactivate()
	settingsButton:Deactivate()
	quitButton:Deactivate()
    
	muteMusicButton:Deactivate()
	muteSfxButton:Deactivate()
	toggleFullscreenButton:Deactivate()
	backButton:Deactivate()
end

-- Creates a new "button" object.
function NewButton(spriteSelected, spriteUnselected, activateFunc)
	local button = {selected = false}
	
	button.gameObject = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/GenericPauseUI.json")
	assert(button.gameObject ~= nil)
	
	-- Initialize the sprite.
	button.gameObject:GetSprite().id = Resource.FilenameToID(spriteUnselected)
	
	
	function button:Select()
		if (self.selected == false)
		then
			self.selected = true
			self.gameObject:GetSprite().id = Resource.FilenameToID(spriteSelected)
		end
	end
	
	function button:Deselect()
		if (self.selected == true)
		then
			self.selected = false
			self.gameObject:GetSprite().id = Resource.FilenameToID(spriteUnselected)
		end
	end
	
	function button:CallAction()
		activateFunc()
	end
	
	function button:Activate()
		self.gameObject:Activate()
	end
	
	function button:Deactivate()
		self.gameObject:Deactivate()
	end
	
	function button:GetUI()
		return self.gameObject:GetScript("GenericUI.lua")
	end
	
	return button
end

-- Returns true if we toggled.
function CheckForToggle()
	if OnPress(KEY.Escape)
	then
		SetPaused(not IsPaused())
		return true
	end
	
	-- Didn't detect a toggle
	return false
end

-- Switches to the "main" pause page.
function ActivateMain()
end

-- Switches to the "settings" pause page.
function ActivateSettings()
end

function Update()
	-- Don't do our stuff if we're switching to the other state.
	if (CheckForToggle())
	then
		return
	end
	
	DeactivateAllButtons()
	pauseBackground:Deactivate()
end

function PausedUpdate()
	-- Don't do our stuff if we're switching to the other state.
	if (CheckForToggle())
	then
		return
	end
	
	pauseBackground:Activate()
	
	for i,v in ipairs(menuItems)
	do
		v:Activate()
		if (i == itemSelected)
		then
			v:Select()
		else
			v:Deselect()
		end
		
		local uiScript = v:GetUI()
		uiScript.scale_y = 0.7
		uiScript.offset_y = 1.5 - i * 0.85
	end
	
end
