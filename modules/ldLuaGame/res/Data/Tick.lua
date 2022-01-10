include("Data/DeltaTimeCounter.lua")

local ms_per_tick = 1000.0 / 60.0

class "Tick"

function Tick:__init()
	self.delta_time_counter = DeltaTimeCounter()

	self.unprocessed = 0
	self.ticks = 0

	self.OnTick = nil
end

function Tick:SetOnTick(OnTick)
	self.OnTick = OnTick
end

function Tick:Update()
	assert(self.OnTick)
	local delta, delta_msec = self.delta_time_counter:Update()

	self.unprocessed = self.unprocessed + (delta_msec) / ms_per_tick
	while self.unprocessed >= 1.0 do
		self.OnTick(ms_per_tick * 0.001)
		self.unprocessed = self.unprocessed - 1.0
	end
end