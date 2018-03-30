--[[
FILE: PauseMenu.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- Keep track of active menu items and which one is selected.
local menuItems = {}
local itemSelected = 1

-- All the ways to check for input.
local inputHandlers = {}

-- Big see-through pause background.
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

-----------------------------------------------------------------------
-- GENERAL HELPERS

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

function ActivateButtons(buttonTable)
	for _, button in pairs(buttonTable)
	do
		button:Activate()
	end
end

-----------------------------------------------------------------------
-- "OBJECT" BUILDERS

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

-- Keyboard-based input handler.
function NewKeyboardInput(upKey, downKey, confirmKey, backKey, pauseKey)
	input = {}
	
	-- Returns a function wrapper checking for a key
	local function KeyCheck(key)
		return function(self) return OnPress(key) end
	end
	
	input.UpPressed = KeyCheck(upKey)
	input.DownPressed = KeyCheck(downKey)
	input.ConfirmPressed = KeyCheck(confirmKey)
	input.BackPressed = KeyCheck(backKey)
	input.PausePressed = KeyCheck(pauseKey)
	
	-- Dummy function. Don't do anything.
	function input:Finalize() end
	
	return input
end

-- Controller-based input handler.
function NewControllerInput(controllerID)
	input = {}
	input.id = controllerID
	
	input.axisLast = 0
	
	local DEADZONE = 0.4
	local VERTICAL_AXIS = 1
	
	function input:UpPressed()
		return (GamepadGetAxis(self.id, VERTICAL_AXIS) > DEADZONE and self.axisLast < DEADZONE)
	end
	
	function input:DownPressed()
		return (GamepadGetAxis(self.id, VERTICAL_AXIS) < -DEADZONE and self.axisLast > -DEADZONE)
	end
	
	function input:ConfirmPressed()
		return GamepadOnPress(self.id, 0) -- A button.
	end
	
	function input:BackPressed()
		return GamepadOnPress(self.id, 1) -- B button.
	end
	
	function input:PausePressed()
		return GamepadOnPress(self.id, 7) -- Start button.
	end
	
	function input:Finalize()
		self.axisLast = GamepadGetAxis(self.id, VERTICAL_AXIS)
	end
	
	return input
end

-----------------------------------------------------------------------
-- MENU ITEM ACTIONS

function Resume()
	SetPaused(false)
end

-- Switches to the "main" pause page.
function ActivateMain()
	DeactivateAllButtons()
	menuItems = {resumeButton, restartButton, settingsButton, quitButton}
	itemSelected = 1
	ActivateButtons(menuItems)
	inSettings = false
end

-- Switches to the "settings" pause page.
function ActivateSettings()
	DeactivateAllButtons()
	menuItems = {muteMusicButton, muteSfxButton, toggleFullscreenButton, backButton}
	itemSelected = 1
	ActivateButtons(menuItems)
	inSettings = true
end

-- Opens up the restart confirmation dialog.
function Restart()
	-- TODO: Implement this.
	print("Restart button not yet implemented.")
end

-- Opens up the quit confirmation dialog.
function QuitButton()
	-- TODO: Implement this.
	print("Quit button not yet implemented.")
end

function ToggleSFX()
	-- TODO: Implement this.
	print("SFX toggle not yet implemented.")
end

function ToggleMusic()
	-- TODO: Implement this.
	print("Music toggle not yet implemented.")
end

function ToggleFullscreen()
	-- TODO: Implement this.
	print("Fullscreen toggle not yet implemented.")
end

-----------------------------------------------------------------------
-- MAIN FUNCTIONS

function Start()
	pauseBackground = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/PauseBackground.json", true)
	
	-- Register the input handlers
	inputHandlers = 
	{
		NewKeyboardInput(KEY.W, KEY.S, KEY.Space, KEY.Q, KEY.Escape), 
		NewControllerInput(0),
		NewControllerInput(1),
		NewControllerInput(2),
		NewControllerInput(3),
	}
	
	-- Main pause menu buttons
	resumeButton = NewButton("treeboy.png", "ground.png", Resume)
	restartButton = NewButton("treeboy.png", "ground.png", Restart)
	settingsButton = NewButton("treeboy.png", "ground.png", ActivateSettings)
	quitButton = NewButton("treeboy.png", "ground.png", QuitButton)
	
	-- Settings pause menu buttons.
	muteMusicButton = NewButton("treeboy.png", "ground.png", ToggleMusic)
	muteSfxButton = NewButton("treeboy.png", "ground.png", ToggleSFX)
	toggleFullscreenButton = NewButton("treeboy.png", "ground.png", ToggleFullscreen)
	backButton = NewButton("treeboy.png", "ground.png", ActivateMain)
	
	ActivateMain()
	
end

-- Returns true if we toggled.
function CheckForToggle()
	-- Check each input handler.
	for _, handler in pairs(inputHandlers)
	do
		if handler:PausePressed()
		then
			SetPaused(not IsPaused())
			ActivateMain()
			return true
		end
	end
	
	-- Didn't detect a toggle
	return false
end

function UpdateSelectedMenuItem()
	local shiftAmount = 0
	
	-- Get how much to shift. (Will usually just be one up or down.
	for _, handler in pairs(inputHandlers)
	do
		if (handler:UpPressed())
		then
			shiftAmount = shiftAmount - 1
		end
		if (handler:DownPressed())
		then
			shiftAmount = shiftAmount + 1
		end
	end
	
	local newSelected = ((itemSelected + shiftAmount - 1) % #menuItems) + 1
	
	menuItems[itemSelected]:Deselect()
	menuItems[newSelected]:Select()
	
	itemSelected = newSelected	
end

-- Returns true if there was a confirmation.
function CheckForConfirm()
	for _, handler in pairs(inputHandlers)
	do
		if (handler:ConfirmPressed())
		then
			menuItems[itemSelected]:CallAction()
			return true
		end
	end
	
	-- No confim button pressed.
	return false
end

function FinalizeInputHandlers()
	for _, handler in pairs(inputHandlers)
	do
		handler:Finalize()
	end
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
	
	if (inSettings)
	then
		for _, handler in pairs(inputHandlers)
		do
			if (handler:BackPressed())
			then
				ActivateMain()
				break
			end
		end
	end
	
	if (not CheckForConfirm())
	then
		UpdateSelectedMenuItem()
	end
	
	-- TODO: Do this at an appropriate time rather than every frame.
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
	
	FinalizeInputHandlers()	
end
