class "Tween"

function Tween:__init(from, to, duration, cyclic, Easing, Setter, OnFinish)
	self.from = from
	self.to = to
	self.duration = duration
	self.cyclic = cyclic
	self.Easing = Easing
	self.Setter = Setter
	self.OnFinish = OnFinish

	self.time = 0
end

function Tween:_ChangeValue()
	local result = {}
	for k, v in ipairs(self.from) do
		local start_value = v
		local change_value = self.to[k] - v
		local value = self.Easing(self.time, start_value, change_value, self.duration)
		table.insert(result, value)
	end
	self.Setter(table.unpack(result))
end

function Tween:Update(delta)
	self.time = self.time + delta
	if self.time < self.duration then
		self:_ChangeValue()
	else
		self.Setter(table.unpack(self.to))
		if not self.cyclic then
			if self.OnFinish then
				self.OnFinish()
			end
		else
			local temp = self.to
			self.to = self.from
			self.from = temp
			self.time = 0
		end
	end
end

function Tween:SetOnFinish(OnFinish)
	self.OnFinish = OnFinish
end