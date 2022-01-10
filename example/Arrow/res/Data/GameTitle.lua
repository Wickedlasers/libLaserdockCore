class "GameTitle" (Object)

function GameTitle:__init(text)
	Object.__init(self)

	local tl = TextLabel(text, 0.075, Vector(250, 400), Colors.ORANGE)
	self:AddChild(tl)
end