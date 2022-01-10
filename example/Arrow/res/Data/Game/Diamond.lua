class "Diamond" (Object)

function Diamond:__init(position)
	Object.__init(self)

    self.size = 20
    self.position = position

    self:AddAnimation()
end

function Diamond:OnDraw()
    Graphics.render_gradient_circle(self.position.x, self.position.y, self.size, 4, Colors.YELLOW, Colors.ORANGE, math.pi / 2, 4)
end

function Diamond:GetEdges()
    local edges = {}
    table.insert(edges, self.position + Vector(self.size, 0))
    table.insert(edges, self.position + Vector(0, self.size))
    table.insert(edges, self.position + Vector(-self.size, 0))
    table.insert(edges, self.position + Vector(0, -self.size))
    return edges
end

function Diamond:AddAnimation()
    local animation =
        TweenTemplates.ChangeValue(
            self,
            self.size,
            self.size - 3,
            0.5,
            true,
            easing.inElastic,
            function (value) -- OnChangeValue
                self.size = value
            end)
    self:AddComponent("Animation", animation)
end

function Diamond:Move(offset)
    self.position.y = self.position.y - offset.y
end