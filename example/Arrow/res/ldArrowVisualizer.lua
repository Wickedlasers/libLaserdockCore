local date = os.date("*t")

math.randomseed(os.clock() + date.sec + date.min + date.hour + date.day + date.month + date.year)
math.random(); math.random(); math.random()

include("Data/Helpers.lua")
include("Data/Class.lua")
include("Data/Object.lua")
include("Data/Colors.lua")
include("Data/Colorise.lua")
include("Data/State.lua")
include("Data/Vector.lua")
include("Data/Graphics.lua")
include("Data/Sound.lua")
include("Data/Tick.lua")
include("Data/Timer.lua")
include("Data/TextLabel.lua")
include("Data/Game/Main.lua")
include("Data/TweenTemplates.lua")
include("Data/External/easing.lua")
include("Data/Tween.lua")

InputEvents = {
	UpKeyPress = false,
    DownKeyPress = false,
    LeftKeyPress = false,
    RightKeyPress = false
}

function OnKeyLeft(_, key_press)
    InputEvents.LeftKeyPress = key_press
end

function OnKeyRight(_, key_press)
    InputEvents.RightKeyPress = key_press
end

function Handler(message)
	log("Lua: "..message)
	return "Handled this message: " .. message
end

log_ = function (...)
	local result_str = ""
	for k, v in ipairs({...}) do
		result_str = result_str..tostring(v).." "
	end
	log(result_str)
end

print = function(...)
    local result_str = ""
    for k, v in ipairs({...}) do
        result_str = result_str..tostring(v).." "
    end
    log(result_str)
end

local last_timer = nil
local frames = 0
local ticks_counter = 0

local game_time = 0

GetGameTime = function ()
	return game_time
end

-- Вызывает апдейт 60 раз в секунду внезависимости от фпс
local tick = Tick()
tick:SetOnTick(function (delta)
	game_time = game_time + delta
	Update(delta)
	ticks_counter = ticks_counter + 1
end)

function OnStart()
        StartGame()
end

function OnStop()
	Stop()
end

function OnReset()
	Reset()
end

function InitVis(visualizer_)
    Graphics.init_visualizer(visualizer_)
    Sound.init_visualizer(visualizer_)
    Load()
end

function OnDraw()
	tick:Update()

	-- Log ticks and fps
	frames = frames + 1
	last_timer = last_timer or GetGameTime()
	if (GetGameTime() - last_timer) > 1.0 then
		last_timer = last_timer + 1.0
		-- log_(ticks_counter.." ticks, "..frames.." fps")
		-- print(ticks_counter.." ticks, "..frames.." fps")
		frames = 0
		ticks_counter = 0
	end

	Draw()
end
