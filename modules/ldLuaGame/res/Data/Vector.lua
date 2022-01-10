class "Vector"

function Vector:__init(x, y)
    x = x or 0
    y = y or 0

    self.x = x
    self.y = y
end

function Vector:__add(vector2)
    local x = self.x + vector2.x
    local y = self.y + vector2.y
    return Vector(x, y)
end

function Vector:__sub(vector2)
    local x = self.x - vector2.x
    local y = self.y - vector2.y
    return Vector(x, y)
end

function Vector:__mul(scalar)
    local x = self.x * scalar
    local y = self.y * scalar
    return Vector(x, y)
end

function Vector:__div(scalar)
    local x = self.x / scalar
    local y = self.y / scalar
    return Vector(x, y)
end

function Vector:__tostring()
    return "x: "..self.x.." y: "..self.y
end

function Vector:GetLength()
    return math.sqrt(self.x ^ 2 + self.y ^ 2)
end

function Vector:Normalize()
    local length = self:GetLength()
    if length > 0 then
        self = self * (1 / length)
    end
    return self
end

function Vector:Rotate(angle) -- RADIAN
    if angle ~= 0 then
        local temp_x = 0
        local temp_y = 0
        local cosinus = math.cos(angle)
        local sinus = math.sin(angle)
        temp_x = self.x * cosinus - self.y * sinus
        temp_y = self.x * sinus + self.y * cosinus
        self.x = temp_x
        self.y = temp_y
    end
    return self
end

-- TODO: replace on math.atan2
function Vector:GetAngle()
    if self.x == 0 and self.y == 0 then
        return 0
    elseif self.x > 0 and self.y == 0 then
        return 0
    elseif self.x >= 0 and self.y >= 0 then
        return math.asin(self.y / self:GetLength())
    elseif self.x <= 0 and self.y >= 0 then
        return math.acos(self.x / self:GetLength())
    elseif self.x <= 0 and self.y <= 0 then
        return 2 * math.pi - math.acos(self.x / self:GetLength())
    elseif self.x >= 0 and self.y <= 0 then
        return 2 * math.pi - math.acos(self.x / self:GetLength())
    end
end

function Vector:Clamp(start, finish)
    self.x = self.x < start.x and start.x or self.x
    self.x = self.x > finish.x and finish.x or self.x
    self.y = self.y < start.y and start.x or self.y
    self.y = self.y > finish.y and finish.x or self.y
end