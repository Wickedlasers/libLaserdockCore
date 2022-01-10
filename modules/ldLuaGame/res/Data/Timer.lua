class "Timer"

function Timer:__init(time, OnFinish)
	assert(time > 0)

	self.time = time
	self.OnFinish = OnFinish
end

function Timer:Update(delta)
	if self.time <= 0 then
		self.OnFinish()
	end
	if self.time > 0 then
		self.time = self.time - delta
	end
end