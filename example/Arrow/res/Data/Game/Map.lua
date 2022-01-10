-- TODO : отрисовывать только видимую часть карты

class "Map" (Object)

function Map:__init()
    Object.__init(self)

    local v = Vector(250, 0)
    self.left = Vector(v.x, v.y):Rotate(3 / 4 * math.pi) * -1
    self.right = Vector(v.x, v.y):Rotate(math.pi / 4) * -1

    self.init_point = Vector(400, 700)

    self.points = {}
    self.left_points_arr = {}
    self.right_points_arr = {}

    table.insert(self.points, self.init_point)
    table.insert(self.left_points_arr, Vector(math.random(100, 300), 0))
    table.insert(self.right_points_arr, Vector(math.random(100, 300), 0))

    while #self.points < 20 do
        self:AddNewPoint()
    end
end

function Map:AddNewPoint()
    local point

    local current_point = self.points[1]
    for i = 2, #self.points do
        current_point = current_point + self.points[i]
    end

    while true do
        if math.random(1, 2) == 1 then
            point = self.right
        else
            point = self.left
        end

        if current_point.x + point.x > 50 and current_point.x + point.x < 750 then
            break
        end
    end

    table.insert(self.points, point)
    table.insert(self.left_points_arr, Vector(math.random(100, 300), 0))
    table.insert(self.right_points_arr, Vector(math.random(100, 300), 0))
end

function Map:OnDraw()
    local left_points_arr, right_points_arr = self:GetBorderLineStrips()
    -- print(#left_points_arr, #right_points_arr)
    -- Graphics.render_linestrip(left_points_arr, Colors.RED)
    -- Graphics.render_linestrip(right_points_arr, Colors.RED)

    local colors = self:GetColors(#left_points_arr)

    Graphics.render_gradient_linestrip(left_points_arr, colors)
    Graphics.render_gradient_linestrip(right_points_arr, colors)
end

function Map:GetColors(amount)
    local result = {}

    while #result < amount do
        table.insert(result, Colors.MAGENTA)
        table.insert(result, Colors.YELLOW)
        table.insert(result, Colors.RED)
        table.insert(result, Colors.CYAN)
    end

    return result
end

function Map:GetBorderLineStrips()
    local left_points_arr = {}
    local right_points_arr = {}

    local t1 = QTime:currentTime():msec()

    local current_point = self.points[1]
    for i = 1, #self.points do
        if i > 1 then
            current_point = current_point + self.points[i]
        end

        local result_left_point = current_point - self.left_points_arr[i]
        if result_left_point.y > -300 and result_left_point.y < 1600 then
            table.insert(left_points_arr, current_point - self.left_points_arr[i])
        end

        local result_right_point = current_point + self.right_points_arr[i]
        if result_right_point.y > -300 and result_right_point.y < 1600 then
            table.insert(right_points_arr, current_point + self.right_points_arr[i])
        end
    end

    -- local t2 = QTime:currentTime():msec()
    -- local delta = t2 - t1
    -- if delta > 2 then
    --     print(delta, #self.points)
    -- end

    return left_points_arr, right_points_arr
end

function Map:Move(offset)
    self.init_point.y = self.init_point.y - offset.y

    local current_point = self.points[1]
    for i = 2, #self.points do
        current_point = current_point + self.points[i]
    end
    -- print(current_point.y)
    if current_point.y > 0 then
        self.init_point = self.init_point + self.points[2]
        self.points[2] = self.init_point

        table.remove(self.points, 1)
        table.remove(self.right_points_arr, 1)
        table.remove(self.left_points_arr, 1)

        self:AddNewPoint()
    end
end