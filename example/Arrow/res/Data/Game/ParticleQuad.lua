class "ParticleQuad" (Object)

function ParticleQuad:__init(position, size)
	Object.__init(self)

	assert(size.x == size.y)

	self.position = position
	self.size = size
	self.color = Colors.WHITE
end

function ParticleQuad:OnDraw()
	Graphics.render_circle(self.position.x, self.position.y, self.size.x / 2, 4, self.color, math.pi / 4, 1)
end

function ParticleQuad:SetColor(color)
	self.color = color
end

function ParticleQuad:GetColor()
	return self.color
end

function ParticleQuad:SetSize(size)
	self.size = size
end