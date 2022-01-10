class "Color"

function Color:__init(r, g, b)
	assert(r >= 0 and r <= 255)
	assert(g >= 0 and g <= 255)
	assert(b >= 0 and b <= 255)

	self.r = r
	self.g = g
	self.b = b
end

function Color:Set()
	-- TODO
end

function Color:Get()
	return {self.r, self.g, self.b}
end