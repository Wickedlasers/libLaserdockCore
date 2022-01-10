include("Data/Game/World.lua")
include("Data/GameTitle.lua")
include("Data/Pause.lua")

SFX = {}
SFX.soundtrack = 1
SFX.coin = 2
SFX.beep = 3
SFX.collision = 4
SFX.hit = 5

local sounds_cached

local world

local pause
local game_title

-- Вызывается 1 раз на старте
function Load()
    LoadSound()
end

function LoadSound(visualizer)
	if not sounds_cached then
        Sound.Add(SFX.soundtrack, "arrow/soundtrack.wav")
                Sound.Add(SFX.coin, "arrow/coin.wav")
                Sound.Add(SFX.collision, "arrow/collision.wav")

        Sound.SetLoops(SFX.soundtrack, -2)
        sounds_cached = true
	end
end

function StartGame()
	if world then
		world.pipe_render:Set(true)
		world.pipe_update:Set(true)
                if pause then
                        pause = nil
                        Sound.Play(SFX.soundtrack)
                end
	else
		world = World()
	end
end

function Stop()
	if world then
    	world.pipe_update:Set(false)
    	pause = Pause(Vector(300, 700))
        -- Sound.Stop(SFX.soundtrack)
    end
end

function Reset()
    game_title = nil
	world = nil
	pause = nil

	StartGame()
end

function Update(delta)
	if world then
		world:Update(delta)
	end
	if pause then
		pause:Update(delta)
	end
    if not world then
        if game_title then
            game_title:Update(delta)
        else
            game_title = GameTitle("Arrow!")
        end
    end
end

function Draw()
	if world then
		world:Draw()
	end
	if pause then
		pause:Draw()
	end
    if not world and game_title then
        game_title:Draw()
    end
end
