--[[
FILE: InputHandler.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--[[ CONTROLS (REMAP PER PLAYER IN EDITOR) ]]--

PLAYER_INPUT_NUM = 0 -- Player number

DEADZONE = 0.5

UseKeyboard = false

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
	if(UseKeyboard == false)
	then
		GetInputGamepad()
	end

	NormInput()
end

function ResetInput()
	horizontalAxis = 0
	verticalAxis   = 0
	onJumpPress = false
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
	if (GamepadIsPressed(PLAYER_INPUT_NUM, JUMP))
	then
		if(jumpPressed == false)
		then
			onJumpPress = true
		else
			onJumpPress = false
		end

		jumpPressed = true
	else
		jumpPressed = false
	end

	-- Toss
	if (GamepadIsPressed(PLAYER_INPUT_NUM, TOSS))
	then
		attackPressed = true
	else
		attackPressed = false
	end

end -- fn end



function GetKeyboardInput()

	-- Movement
	if (IsPressed(KEY_RIGHT))
	then
		horizontalAxis = 1
	elseif (IsPressed(KEY_LEFT))
	then
		horizontalAxis = -1
	else
		horizontalAxis = 0
	end

	if (IsPressed(KEY_UP))
	then
		verticalAxis = 1
	elseif (IsPressed(KEY_DOWN))
	then
		verticalAxis = -1
	else
		verticalAxis = 0
	end

	-- Jumps
	if (IsPressed(KEY_JUMP))
	then
		if(jumpPressed == false)
		then
			onJumpPress = true
		else
			onJumpPress = false
		end

		jumpPressed = true
	else
		jumpPressed = false
	end

	-- Attack
	if (IsPressed(KEY_ATTACK))
	then
		attackPressed = true
	else
		attackPressed = false
	end

	-- Toss
	if (IsPressed(KEY_TOSS))
	then
		tossPressed = true
	else
		tossOther = false
	end

	UseKeyboard = jumpPressed or onJumpPress or attackPressed or tossPressed or (horizontalAxis ~= 0) or (verticalAxis ~= 0)

end
