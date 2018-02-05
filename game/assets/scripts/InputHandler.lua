--[[
FILE: InputHandler.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--[[ CONTROLS (REMAP PER PLAYER IN EDITOR) ]]--

PLAYER_INPUT_NUM = 0 -- Player number

DEADZONE = 0.5

JUMP	 = 0 -- A
ATTACK   = 2 -- X
TOSS	 = 3 -- Y
HORIZONTAL_AXIS = 0 -- LeftAnalog_X
VERTICAL_AXIS   = 0 -- LeftAnalog_Y

KEY_JUMP   = 87 -- W
KEY_UP     = 87 -- W
KEY_LEFT   = 65 -- A
KEY_DOWN   = 83 -- S
KEY_RIGHT  = 68 -- D
KEY_TOSS   = 84 -- T
KEY_ATTACK = 89 -- Y

jumpPressed    = false
attackPressed  = false
tossPressed    = false

onJumpPress	   = false

horizontalAxis = 0
verticalAxis   = 0

function EarlyUpdate()
	ResetInput()

	GetKeyboardInput()
	GetInputGamepad()

	NormInput()
end

function ResetInput()
	horizontalAxis = 0
	verticalAxis   = 0
	
	jumpPressed    = false
	attackPressed  = false
	tossPressed    = false
	
	onJumpPress	   = false
end

function NormInput()
	if(horizontalAxis > 1)  then horizontalAxis = 1 end
	if(horizontalAxis < -1) then horizontalAxis = -1 end

	if(verticalAxis > 1)  then verticalAxis = 1 end
	if(verticalAxis < -1) then verticalAxis = -1 end
end

-- Gamepad input
function GetInputGamepad()
	-- Movement
	if (GamepadGetAxis(PLAYER_INPUT_NUM, HORIZONTAL_AXIS) > DEADZONE)
	then
		horizontalAxis = horizontalAxis + GamepadGetAxis(PLAYER_INPUT_NUM, HORIZONTAL_AXIS)
	elseif (GamepadGetAxis(PLAYER_INPUT_NUM, HORIZONTAL_AXIS) < -DEADZONE)
	then
		horizontalAxis = horizontalAxis + GamepadGetAxis(PLAYER_INPUT_NUM, HORIZONTAL_AXIS)
	else
		--horizontalAxis = 0
	end

	if (GamepadGetAxis(PLAYER_INPUT_NUM, VERTICAL_AXIS) > DEADZONE)
	then
		verticalAxis = verticalAxis + GamepadGetAxis(PLAYER_INPUT_NUM, VERTICAL_AXIS)
	elseif (GamepadGetAxis(PLAYER_INPUT_NUM, VERTICAL_AXIS) < -DEADZONE)
	then
		verticalAxis = verticalAxis + GamepadGetAxis(PLAYER_INPUT_NUM, VERTICAL_AXIS)
	else
		--verticalAxis = 0
	end

	-- Jump
	if(GamepadOnPress(PLAYER_INPUT_NUM, JUMP))
	then
		onJumpPress = true
	end

	if (GamepadIsHeld(PLAYER_INPUT_NUM, JUMP))
	then
		jumpPressed = true
	end

	-- Toss
	if (GamepadIsHeld(PLAYER_INPUT_NUM, TOSS))
	then
		tossPressed = true
	end

	-- Attack
	if (GamepadIsHeld(PLAYER_INPUT_NUM, ATTACK))
	then
		attackPressed = true
	end

end -- fn end



function GetKeyboardInput()

	-- Movement
	if (IsHeld(KEY_RIGHT))
	then
		horizontalAxis = 1
	elseif (IsHeld(KEY_LEFT))
	then
		horizontalAxis = -1
	else
		horizontalAxis = 0
	end

	if (IsHeld(KEY_UP))
	then
		verticalAxis = 1
	elseif (IsHeld(KEY_DOWN))
	then
		verticalAxis = -1
	else
		verticalAxis = 0
	end

	-- Jumps
	if(OnPress(KEY_JUMP))
	then
		onJumpPress = true
	end

	if (IsHeld(KEY_JUMP))
	then
		jumpPressed = true
	end

	-- Attack
	if (IsHeld(KEY_ATTACK))
	then
		attackPressed = true
	end

	-- Toss
	if (IsHeld(KEY_TOSS))
	then
		tossPressed = true
	end

end
