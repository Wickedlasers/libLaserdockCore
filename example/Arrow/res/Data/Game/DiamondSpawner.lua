include("Data/Game/Diamond.lua")

class "DiamondSpawner" (Object)

function DiamondSpawner:__init(GetBorderLines)
    Object.__init(self)

    self.GetBorderLines = GetBorderLines

    self.spawn_interval_distance = 1000-- расстояние в пикселях

    self.distance_counter = self.spawn_interval_distance
end

function DiamondSpawner:UpdateSpawnInterval(offset)
    if self.distance_counter < 0 then
        self.distance_counter = self.spawn_interval_distance
        self:_SpawnDiamond()
    end
    self.distance_counter = self.distance_counter + offset.y

    local children = self:GetChildren()
    children:ForEach(function (child)
        child:Move(offset)
    end)

    self:RemoveDiamondsBeyondBorder()
end

function DiamondSpawner:_SpawnDiamond()
    local left_border_lines, right_border_lines = self.GetBorderLines()
    local position = (left_border_lines[#left_border_lines] + right_border_lines[#right_border_lines - 1]) / 2
    -- print(position)
    local diamond = Diamond(position)
    self:AddChild(diamond)
end

function DiamondSpawner:RemoveDiamondsBeyondBorder()
    local result = {}
    local children = self:GetChildren()
    local counter = 0
    children:ForEach(function (child)
        if child.position.y > 1000 then
            table.insert(result, child)
        end
        counter = counter + 1
    end)
    for i = 1, #result do
        self:RemoveChild(result[i])
    end
end