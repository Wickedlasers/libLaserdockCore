class "CollisionHandler"

-- point: Vector
-- triangle: array: A,B,C vectors
local function IsPointBelongToTriangle(point, triangle)
    local x = {}
    local y = {}

    x[0] = math.ceil(point.x)
    x[1] = math.ceil(triangle[1].x)
    x[2] = math.ceil(triangle[2].x)
    x[3] = math.ceil(triangle[3].x)

    y[0] = math.ceil(point.y)
    y[1] = math.ceil(triangle[1].y)
    y[2] = math.ceil(triangle[2].y)
    y[3] = math.ceil(triangle[3].y)

    local a = (x[1] - x[0]) * (y[2] - y[1]) - (x[2] - x[1]) * (y[1] - y[0])
    local b = (x[2] - x[0]) * (y[3] - y[2]) - (x[3] - x[2]) * (y[2] - y[0])
    local c = (x[3] - x[0]) * (y[1] - y[3]) - (x[1] - x[3]) * (y[3] - y[0])

    if ((a >= 0 and b >= 0 and c >= 0) or (a <= 0 and b <= 0 and c <= 0)) then
        return true
    else
        return false
    end
end

local function IsIntersectionExistBetweenTwoLines(l1, l2)
    local function FindIntersection(l1, l2)
        local a1 = l1.finish.y - l1.start.y
        local b1 = l1.start.x - l1.finish.x
        local c1 = a1 * l1.start.x + b1 * l1.start.y

        local a2 = l2.finish.y - l2.start.y
        local b2 = l2.start.x - l2.finish.x
        local c2 = a2 * l2.start.x + b2 * l2.start.y

        local delta = a1 * b2 - a2 * b1

        return Vector((b2 * c1 - b1 * c2) / delta, (a1 * c2 - a2 * c1) / delta)
    end

    local point = FindIntersection(l1, l2)

    -- Особенности lua,  проверка значений inf и nan
    if
        point.x == math.huge or
        point.x == -math.huge or
        point.y == math.huge or
        point.y == -math.huge or
        point.x ~= point.x or
        point.y ~= point.y
    then
        return false
    else
        local l1_start_x = l1.start.x
        local l1_finish_x = l1.finish.x
        local l2_start_x = l2.start.x
        local l2_finish_x = l2.finish.x
        local l1_start_y = l1.start.y
        local l1_finish_y = l1.finish.y
        local l2_start_y = l2.start.y
        local l2_finish_y = l2.finish.y
        -- корректируем, чтобы l1.start.x всегда была меньше чем l1.finish.x, аналогично и по y оси, иначе проверка будет работать некорректно
        if l1.start.x >= l1.finish.x then
            local temp = l1_start_x
            l1_start_x = l1_finish_x
            l1_finish_x = temp
        end
        if l2_start_x >= l2_finish_x then
            local temp = l2_start_x
            l2_start_x = l2_finish_x
            l2_finish_x = temp
        end
        if l1_start_y >= l1_finish_y then
            local temp = l1_start_y
            l1_start_y = l1_finish_y
            l1_finish_y = temp
        end
        if l2_start_y >= l2_finish_y then
            local temp = l2_start_y
            l2_start_y = l2_finish_y
            l2_finish_y = temp
        end

        -- print(point.x >= l1.start.x, point.x, ">=", l1.start.x)
        -- print(point.x <= l1.finish.x, point.x, "<=", l1.finish.x)
        -- print(point.x >= l2.start.x, point.x, ">=", l2.start.x)
        -- print(point.x <= l2.finish.x, point.x, "<=", l2.finish.x)

        return
            (point.x >= l1_start_x and point.x <= l1_finish_x and point.x >= l2_start_x and point.x <= l2_finish_x) and
            (point.y >= l1_start_y and point.y <= l1_finish_y and point.y >= l2_start_y and point.y <= l2_finish_y)
    end
end

function CollisionHandler:__init(OnBorderCollision, OnDiamondCollision)
    self.OnBorderCollision = OnBorderCollision
    self.OnDiamondCollision = OnDiamondCollision
end

function CollisionHandler:CheckHeadWithBorder(head_lines, left_border_points, right_border_points)
    local border_lines = {}
    for i = 1, #left_border_points - 1 do
        table.insert(border_lines, {
            start = Vector(left_border_points[i].x, left_border_points[i].y),
            finish = Vector(left_border_points[i + 1].x, left_border_points[i + 1].y)
        })
    end
    for i = 1, #right_border_points - 1 do
        table.insert(border_lines, {
            start = Vector(right_border_points[i].x, right_border_points[i].y),
            finish = Vector(right_border_points[i + 1].x, right_border_points[i + 1].y)
        })
    end

    for i = 1, #head_lines do
        local l1 = head_lines[i]
        for j = 1, #border_lines do
            local l2 = border_lines[j]
            if IsIntersectionExistBetweenTwoLines(l1, l2) then
                if self.OnBorderCollision then
                    self.OnBorderCollision()
                end
                return
            end
        end
    end
end

function CollisionHandler:CheckHeadWithDiamond(head_points, diamonds)
    for i = 1, #head_points - 1 do -- -1 Потому что 4 точки нужны для построения кривой, но сам треугольник|голова задается тремя точками
        local is_belong_to_diamond
        local diamond
        local position = head_points[i]
        diamonds:ForEach(function (d)
            if not diamond then
                local diamond_edges = d:GetEdges()
                local t1 = {diamond_edges[1], diamond_edges[2], diamond_edges[3]}
                local t2 = {diamond_edges[3], diamond_edges[4], diamond_edges[1]}
                if
                    IsPointBelongToTriangle(position, t1) or
                    IsPointBelongToTriangle(position, t2) or
                    IsPointBelongToTriangle(d.position, {head_points[1], head_points[2], head_points[3]})
                then
                    diamond = d
                end
            end
        end)
        if diamond then
            if self.OnDiamondCollision then
                self.OnDiamondCollision(diamond)
            end
            return
        end
    end
end