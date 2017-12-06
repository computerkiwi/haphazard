--[[
FILE: Interactive.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  if (other:HasTag("Player"))
  then
    local platform = GameObject.FindByName("InteractiveGround")
    local groundScript = platform:GetScript("PlatformMove.lua")
    local left = -2
    groundScript.EnablePlatform(left, 1.9)
    groundScript.SetTimer(0.03)

	local platform = GameObject.FindByName("InteractiveWall")
    local wallScript = platform:GetScript("PlatformMove.lua")
    local down = -1
    wallScript.EnablePlatform(down, 10)
    wallScript.SetTimer(0.03)

    local platform = GameObject.FindByName("InteractiveStairs1")
    local stairs1_Script = platform:GetScript("PlatformMove.lua")
    local up = 1
    stairs1_Script.EnablePlatform(up, 1)
    stairs1_Script.SetTimer(0.075)

    local platform = GameObject.FindByName("InteractiveStairs2")
    local stairs2_Script = platform:GetScript("PlatformMove.lua")
    local up = 1
    stairs2_Script.EnablePlatform(up, 2)
    stairs2_Script.SetTimer(0.22)
  end
end -- fn end