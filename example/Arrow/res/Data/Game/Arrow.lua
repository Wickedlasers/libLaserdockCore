-- include("Data/Game/Chain.lua")
include("Data/Game/Triangle.lua")

class "Arrow" (Object)

local LV = Vector(-1, -1):Normalize()
local RV = Vector(1, -1):Normalize()

local directions = {LEFT = 0, RIGHT = 1}
local max_speed = 10
local init_speed = 6
local init_interval = 8

function Arrow:__init(OnMove)
    Object.__init(self)

    self.OnMove = OnMove

    self.position = Vector(400, 400)
    self.head_position = Vector(0, 0)
    self.direction = (Vector(LV.x, LV.y) + Vector(RV.x, RV.y)) / 2
    self.speed = init_speed

    self.path = {}

    self.interval = init_interval
    self.interval = math.ceil(self.interval * (init_speed / self.speed))

    self.chain_length = 0

    self.triangle = Triangle(Vector(self.position.x, self.position.y), Colors.RED, Colors.YELLOW)
    self:AddChild(self.triangle)
    self.triangle:OnUpdate()
end

function Arrow:IncrementLength()
    self.chain_length = self.chain_length + 1
end

function Arrow:ChangeSpeedBy(value)
    self.speed = self.speed + value
    if self.speed > max_speed then
        self.speed = max_speed
    end
    self.interval = math.ceil(init_interval * (init_speed / self.speed))
end

function Arrow:OnDraw()
    local head_position = self:GetHeadPosition()
    -- Graphics.render_circle(head_position.x, head_position.y, 30, 3, Colors.MAGENTA, self.direction:GetAngle())

    -- Отрисовка звеньев туловища
    local prev_chain_position = prev_chain_position or head_position
    local angle = 0

    local max_visible_chains = 10
    local chains_quantity = self.chain_length
    if chains_quantity > max_visible_chains then
        chains_quantity = max_visible_chains
    end

    -- if #self.path >= 200 then
        for i = 1, chains_quantity do
            local position = self.path[#self.path - i * self.interval] + self.position
            angle = (prev_chain_position - position):GetAngle()
            prev_chain_position = position

            Graphics.render_circle(position.x, position.y, 23, 3, Colors.YELLOW, angle, 1)
        end
    -- end
    -- print(chains_quantity)
end

function Arrow:OnUpdate(dt)
    if InputEvents.LeftKeyPress then
        self.direction:Rotate(-0.1)
    elseif InputEvents.RightKeyPress then
        self.direction:Rotate(0.1)
    end

    if self.direction:GetAngle() < LV:GetAngle() then
        self.direction = Vector(LV.x, LV.y)
    elseif self.direction:GetAngle() > RV:GetAngle() then
        self.direction = Vector(RV.x, RV.y)
    end

    local offset

    self.head_position = self.head_position + self.direction * self.speed
    offset = self.direction * self.speed

    self.position = self.position + Vector(0, offset.y * -1)

    if self.OnMove then
        self.OnMove(offset)
    end

    table.insert(self.path, self.head_position)

    if #self.path > 200 then
        table.remove(self.path, 1)
    end
end

function Arrow:GetHeadPosition()
    return self.position + self.head_position
end

function Arrow:GetLines()
    return self.triangle:GetLines()
end

function Arrow:GetPoints()
    return self.triangle:GetPoints()
end