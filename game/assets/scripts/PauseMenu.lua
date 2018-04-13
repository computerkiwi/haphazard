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
local howToPlayButton
local settingsButton
local creditsButton
local mainMenuButton
local quitButton

-- Settings pause menu buttons.
local muteMusicButton
local muteSfxButton
local toggleFullscreenButton
local backButton

local howToPlayObj
local showingHowToPlay = false

local confirmPromptObj
local confirmYesButton
local confirmNoButton
local confirmYesSelected = false

local inSettings = false

local confirming = false
local confirmingAction = nil

hideMenuInEditor = true
-----------------------------------------------------------------------
-- GENERAL HELPERS

function NewPlaysoundFunction(...)
  local sounds = {...}
  return function()
    local soundName = sounds[math.random(1, #sounds)]
	
    PlaySound(soundName, 1, 1, false)
  end
end

PlayMoveSound = NewPlaysoundFunction(
  "menu_move_01.wav",
  "menu_move_02.wav",
  "menu_move_03.wav",
  "menu_move_04.wav",
  "menu_move_05.wav")

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
  howToPlayButton:Deactivate()
  settingsButton:Deactivate()
  creditsButton:Deactivate()
  mainMenuButton:Deactivate()
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

function ConfirmAction(promptTexture, action)
  confirming = true
  confirmingAction = action

  confirmPromptObj:GetSprite().id = Resource.FilenameToID(promptTexture)
  
  menuItems = {confirmYesButton, confirmNoButton}
  itemSelected = 2
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
function NewKeyboardInput(upKey, downKey, leftKey, rightKey, confirmKey, backKey, pauseKey)
	input = {}
	
	-- Returns a function wrapper checking for a key
	local function KeyCheck(key)
		return function(self) return OnPress(key) end
	end
	
	input.UpPressed = KeyCheck(upKey)
	input.DownPressed = KeyCheck(downKey)
	input.LeftPressed = KeyCheck(leftKey)
	input.RightPressed = KeyCheck(rightKey)
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
	
	input.axisLast = 0 -- Vertical axis
  input.axisLastHoriz = 0
	
	local DEADZONE = 0.4
	local VERTICAL_AXIS = 1
	local HORIZONTAL_AXIS = 0
	
	function input:UpPressed()
		return (GamepadGetAxis(self.id, VERTICAL_AXIS) > DEADZONE and self.axisLast < DEADZONE)
	end
	
	function input:DownPressed()
		return (GamepadGetAxis(self.id, VERTICAL_AXIS) < -DEADZONE and self.axisLast > -DEADZONE)
	end
  
	function input:RightPressed()
		return (GamepadGetAxis(self.id, HORIZONTAL_AXIS) > DEADZONE and self.axisLastHoriz < DEADZONE)
	end
	
	function input:LeftPressed()
		return (GamepadGetAxis(self.id, HORIZONTAL_AXIS) < -DEADZONE and self.axisLastHoriz > -DEADZONE)
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
		self.axisLastHoriz = GamepadGetAxis(self.id, HORIZONTAL_AXIS)
	end
	
	return input
end

-----------------------------------------------------------------------
-- TRANSITION CODE

-- (mostly stolen from EndLevelScript.lua)

local TRANSITION_ACTION

function NewLevelTransition(level)
  return function()
    Engine.LoadLevel(level)
  end
end

local transitionTime = 1.8

transitionStarted = false

transitionScreen = nil
transitionSparkles = nil

function UpdateLevelTransition(dt)
	if(transitionStarted)
	then
		transitionTime = transitionTime - dt

		transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))

		if(transitionTime < 0)
		then
			TRANSITION_ACTION()
		end
	end
end

function StartTransition(transitionAction)
  TRANSITION_ACTION = transitionAction
  transitionStarted = true

  transitionScreen = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransitionFast.json")

  transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))
end

-----------------------------------------------------------------------
-- MENU ITEM ACTIONS

function Resume()
	SetPaused(false)
end

-- Switches to the "main" pause page.
function ActivateMain()
	DeactivateAllButtons()
	menuItems = {resumeButton, restartButton, howToPlayButton, settingsButton, creditsButton, mainMenuButton, quitButton}
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

function MainMenu()
  local function ActualMainMenu()
    SetPaused(false)
    StartTransition(NewLevelTransition("MainMenu.json"))
  end

  ConfirmAction("Prompt_QuitToMainMenu.png", ActualMainMenu)
end

-- Opens up the restart confirmation dialog.
function Restart()
  local function ActualRestart()
    SetPaused(false)
    StartTransition(NewLevelTransition(CurrentLevel()))
  end

  ConfirmAction("Prompt_RestartLevel.png", ActualRestart)
end

function HowToPlayButton()
  showingHowToPlay = true
end

function CreditsButton()
  local function Credits()
    SetPaused(false)
    StartTransition(NewLevelTransition("CreditsLevel.json"))
  end

  ConfirmAction("Prompt_Credits.png", Credits)
end

-- Opens up the quit confirmation dialog.
function QuitButton()
  local function ActualQuit()
    SetPaused(false)
    StartTransition(QuitGame)
  end

  ConfirmAction("Prompt_QuitToDesktop.png", ActualQuit)
end

-- Directly call ToggleSFX()
-- Directly call ToggleMusic()
-- Directly call ToggleFullscreen()

function ConfirmYes()
	SetPaused(false)
  confirming = false
  UpdateConfirmDialog(false)
  confirmingAction()
end

function ConfirmNo()
  confirming = false
  ActivateMain()
end

-----------------------------------------------------------------------
-- MAIN FUNCTIONS

function UpdateHowToPlay()
  if (showingHowToPlay)
  then
    howToPlayObj:Activate()
  else
    howToPlayObj:Deactivate()
  end
end

function Start()
	pauseBackground = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/PauseBackground.json", true)
	
	-- Register the input handlers
	inputHandlers = 
	{
		NewKeyboardInput(KEY.W, KEY.S, KEY.A, KEY.D, KEY.Space, KEY.Q, KEY.Escape), 
		NewControllerInput(0),
		NewControllerInput(1),
		NewControllerInput(2),
		NewControllerInput(3),
	}
	
	-- Main pause menu buttons
	resumeButton = NewButton("PauseMenu_Resume_Selected.png", "PauseMenu_Resume_Unselected.png", Resume)
	restartButton = NewButton("PauseMenu_Restart_Selected.png", "PauseMenu_Restart_Unselected.png", Restart)
  howToPlayButton = NewButton("PauseMenu_HowToPlay_Selected.png", "PauseMenu_HowToPlay_Unselected.png", HowToPlayButton)
	settingsButton = NewButton("PauseMenu_Options_Selected.png", "PauseMenu_Options_Unselected.png", ActivateSettings)
	creditsButton = NewButton("PauseMenu_Credits_Selected.png", "PauseMenu_Credits_Unselected.png", CreditsButton)
	mainMenuButton = NewButton("PauseMenu_MainMenu_Selected.png", "PauseMenu_MainMenu_Unselected.png", MainMenu)
	quitButton = NewButton("PauseMenu_Quit_Selected.png", "PauseMenu_Quit_Unselected.png", QuitButton)
	
	-- Settings pause menu buttons.
	muteMusicButton = NewButton("PauseMenu_MuteMusic_Selected.png", "PauseMenu_MuteMusic_Unselected.png", ToggleMusic)
	muteSfxButton = NewButton("PauseMenu_MuteSFX_Selected.png", "PauseMenu_MuteSFX_Unselected.png", ToggleSFX)
	toggleFullscreenButton = NewButton("PauseMenu_ToggleFullscreen_Selected.png", "PauseMenu_ToggleFullscreen_Unselected.png", ToggleFullscreen)
	backButton = NewButton("PauseMenu_Back_Selected.png", "PauseMenu_Back_Unselected.png", ActivateMain)
	
  -- Confirmation stuff.
  confirmPromptObj = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/GenericPauseUI.json", true)
  confirmYesButton = NewButton("Button_Yes_On.png", "Button_Yes_Off.png", ConfirmYes)
  confirmNoButton = NewButton("Button_No_On.png", "Button_No_Off.png", ConfirmNo)
  confirmPromptObj:GetTransform().zLayer = 1501
  confirmYesButton.gameObject:GetTransform().zLayer = 1502
  confirmNoButton.gameObject:GetTransform().zLayer = 1502
  
  howToPlayObj = SpawnAndAttachObject("assets/prefabs/pause_menu_internal/GenericPauseUI.json", true)
  howToPlayObj:GetSprite().id = Resource.FilenameToID("Controls.png")
  howToPlayObj:GetTransform().zLayer = 1501
  local howUI = howToPlayObj:GetScript("GenericUI.lua")
  howUI.scale_y = 4
  howUI.scale_x = (1429 / 900) * howUI.scale_y
  howToPlayObj:Deactivate()
  
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
      SetQuietMusic(IsPaused())
			ActivateMain()
      PlayMoveSound()
			return true
		end
	end
	
	-- Didn't detect a toggle
	return false
end

-- Returns true if actively confirming
function UpdateConfirmDialog(confirmActive)
  if (confirmActive == nil)
  then
    confirmActive = confirming
  end

  if (confirmActive)
  then
    -- Make sure all the buttons are active.
    confirmPromptObj:Activate()
    confirmYesButton:Activate()
    confirmNoButton:Activate()
    
    -- Position all the confirmation UI.
    do
      local confirmUI = confirmPromptObj:GetScript("GenericUI.lua")
      confirmUI.scale_y = 3
      confirmUI.scale_x = (512 / 384) * confirmUI.scale_y
      
      local yesUI = confirmYesButton:GetUI()
      yesUI.scale_y = 0.5
      yesUI.scale_x = (192 / 80) * yesUI.scale_y
      yesUI.offset_y = -0.8
      yesUI.offset_x = -0.9
      
      local noUI = confirmNoButton:GetUI()
      noUI.scale_y = 0.5
      noUI.scale_x = (192 / 80) * noUI.scale_y
      noUI.offset_y = -0.8
      noUI.offset_x = 0.9
    end
    
    for _, handler in pairs(inputHandlers)
    do
      if (handler:LeftPressed() or handler:LeftPressed())
      then
        
      end
    end
    
    return true
  else
    confirmPromptObj:Deactivate()
    confirmYesButton:Deactivate()
    confirmNoButton:Deactivate()
    
    return false
  end
end

function UpdateSelectedMenuItem()
	local shiftAmount = 0
	
  -- Left and right when confirming.
  local forwardPressed
  local backPressed
  if (confirming)
  then
    forwardPressed = "RightPressed"
    backPressed = "LeftPressed"
  else
    forwardPressed = "DownPressed"
    backPressed = "UpPressed"
  end
  
	-- Get how much to shift. (Will usually just be one up or down.
	for _, handler in pairs(inputHandlers)
	do
		if (handler[backPressed](handler))
		then
			shiftAmount = shiftAmount - 1
		end
		if (handler[forwardPressed](handler))
		then
			shiftAmount = shiftAmount + 1
		end
	end
	
  if (shiftAmount ~= 0)
  then
    PlayMoveSound()
    showingHowToPlay = false
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
      PlayMoveSound()
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

function EditorUpdate()
  if (EditorIsOpen() and hideMenuInEditor)
  then
    pauseBackground:GetScript("GenericUI.lua").offset_y = 99999
    
    EditHide(resumeButton)
    EditHide(restartButton)
    EditHide(howToPlayButton)
    EditHide(settingsButton)
    EditHide(creditsButton)
    EditHide(mainMenuButton)
    EditHide(quitButton)
    
    EditHide(muteMusicButton)
    EditHide(muteSfxButton)
    EditHide(toggleFullscreenButton)
    EditHide(backButton)
  else
    pauseBackground:GetScript("GenericUI.lua").offset_y = 0
  end
end

function EditHide(button)
  local uiScript = button:GetUI()
  uiScript.offset_y = 99999
end

function Update(dt)
  -- If we're currently transitioning, don't let anyone pause again.
  if (transitionStarted)
  then
    UpdateLevelTransition(dt)
  end

	-- Don't do our stuff if we're switching to the other state.
	if (CheckForToggle())
	then
    UpdatePositions()
		return
	end
  
  confirming = false
  UpdateConfirmDialog()
  
  showingHowToPlay = false
  UpdateHowToPlay()
  
	DeactivateAllButtons()
	pauseBackground:Deactivate()
	
  EditorUpdate()
end

function UpdatePositions()
  if (not UpdateConfirmDialog())
  then
    for i,v in ipairs(menuItems)
    do
      v:Activate()
      if (i == itemSelected)
      then
        v:Select()
      else
        v:Deselect()
      end
      
      local BUTTON_HEIGHT = .5
      local BUTTON_SPACING = 0.02
      local BUTTON_LEFT_X = -3.8
      local BUTTON_BOT_Y = -2.5
      
      local BUTTON_TOP_Y = BUTTON_BOT_Y + #menuItems *(BUTTON_HEIGHT + BUTTON_SPACING)
      
      local uiScript = v:GetUI()
      uiScript.scale_y = BUTTON_HEIGHT
      uiScript.scale_x = 4 * uiScript.scale_y
      uiScript.offset_x = BUTTON_LEFT_X
      uiScript.offset_y = BUTTON_TOP_Y - i * (BUTTON_HEIGHT + BUTTON_SPACING)
    end
  end
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
	
  UpdateHowToPlay()
  
	-- TODO: Do this at an appropriate time rather than every frame.
	pauseBackground:Activate()
  UpdatePositions()
	
	FinalizeInputHandlers()	
	
  EditorUpdate()
end
