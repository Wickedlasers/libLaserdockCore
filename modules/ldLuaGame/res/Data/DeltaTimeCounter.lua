class "DeltaTimeCounter"

function DeltaTimeCounter:__init()
	self.prev_msec = nil
end

function DeltaTimeCounter:Update()
	local current_msec = QTime:currentTime():msec()
	local delta
	local delta_msec
	self.prev_msec = self.prev_msec or current_msec
	if self.prev_msec ~= current_msec then
		if math.abs(current_msec - self.prev_msec) > 500 then -- 500ms MAGIC CONST
			delta_msec = (1000 - self.prev_msec + current_msec)
			delta = delta_msec * 0.001
		else
			delta_msec = (current_msec - self.prev_msec)
			delta = (current_msec - self.prev_msec) * 0.001
		end
	else
		delta_msec = 1000.0 / 30.0
		delta = 1.0 / 30.0 -- MAGIC CONST
	end
	self.prev_msec = current_msec
	-- log("delta: "..tostring(delta).." FPS: "..tostring(1 / delta))
	return delta, delta_msec
end