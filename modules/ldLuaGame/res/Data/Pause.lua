class "Pause" (Object)

function Pause:__init(pos)
	Object.__init(self)

	local tl = TextLabel("PAUSE", 0.075, pos, 0xf15e75) -- 0xf15e75: color
	self:AddChild(tl)
end
