--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

KeyID = "DefaultGem"
particlePrefabName = "assets/prefabs/Particles_Gem.json"
pairedGnome = ""

local particles = nil

function Start()
	particles = GameObject.LoadPrefab(particlePrefabName)
	particles:GetTransform().position = this:GetTransform().position
end

function CorrectGnome(name)

	return (pairedGnome == "") or (pairedGnome == name)

end

local thisCollected = false
function OnCollisionEnter(other)
  -- Player takes gem
  if (not thisCollected and other:HasTag("Player") and other:GetScript("GnomeStatus.lua").hasGem == false and CorrectGnome(other:GetName()))
  then
		PlaySound("Gem.wav", 1, 1, false)

		thisCollected = true
	
    -- Set gnome to have gem
		other:GetScript("GnomeStatus.lua").hasGem = true
		other:GetScript("GnomeStatus.lua").GemID = KeyID

    -- Deactivate the object
    this:Deactivate()
	particles:Destroy()
  end
end
