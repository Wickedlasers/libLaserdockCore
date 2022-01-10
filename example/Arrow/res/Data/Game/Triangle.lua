class "Triangle" (Object)

function Triangle:__init(position, color1, color2)
	Object.__init(self)

    self.position = position
    self.color1 = Colors.RED
    self.color2 = Colors.YELLOW

    self.points = {}
    self.points[1] = Vector(40, 0)
    self.points[2] = Vector(-10, -25)
    self.points[3] = Vector(-10, 25)

    self.angle = 0
end

function Triangle:OnUpdate()
    local parent_position = self:GetParent():GetHeadPosition()
    local parent_angle = self:GetParent().direction:GetAngle()

    self.position = Vector(parent_position.x, parent_position.y)

    local delta_angle = parent_angle - self.angle
    self.angle = parent_angle

    for i = 1, #self.points do
        self.points[i] = ((self.points[i] + self.position) - self.position):Rotate(delta_angle)
    end
end

function Triangle:OnDraw()
    local points = self:GetPoints()
    -- Graphics.render_gradient_linestrip(points, {self.color1, self.color2, self.color1, self.color2})
    Graphics.render_linestrip(points, Colors.MAGENTA, 1)
end

function Triangle:GetPoints()
    local points = {}
    for i = 1, #self.points do
        points[i] = self.points[i] + self.position
    end
    table.insert(points, self.points[1] + self.position)

    return points
end

function Triangle:GetLines()
    local points = self:GetPoints()

    local lines = {}
    table.insert(lines, {
        start = Vector(points[1].x, points[1].y),
        finish = Vector(points[2].x, points[2].y)
    })
    table.insert(lines, {
        start = Vector(points[2].x, points[2].y),
        finish = Vector(points[3].x, points[3].y)
    })
    table.insert(lines, {
        start = Vector(points[3].x, points[3].y),
        finish = Vector(points[1].x, points[1].y)
    })
    return lines
end

function Triangle:SetColors(color1, color2)
    self.color1 = color1
    self.color2 = color2
end