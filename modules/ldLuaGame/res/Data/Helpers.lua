function CheckRectanglesOverlap(a, b)
        return
                not (a.x + a.width < b.x or b.x + b.width < a.x or a.y + a.height < b.y or b.y + b.height < a.y)
end

function CheckCirclesOverlap(a, b)
	return (a.position - b.position):GetLength() < (a.radius + b.radius)
end


-- point -> Vector()
-- rectangle : left top position -> Vector()
--			   right down position -> Vector()
function IsPointInRectangle(point, rectangle)
        return
                point.x >= rectangle.start.x and
                point.x <= rectangle.finish.x and
                point.y >= rectangle.start.y and
                point.y <= rectangle.finish.y
end

function GetRandomRGB()
	local r = math.random(0, 255)
	local g = math.random(0, 255)
	local b = math.random(0, 255)

	local luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b

	while luminance < 125 do
		r = math.random(0, 255)
		g = math.random(0, 255)
		b = math.random(0, 255)
		luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b
	end

	local cls = {r, g, b}
	local colors = {}

	while #colors ~= 3 do
		local index = math.random(1, #cls)
		table.insert(colors, cls[index])
		table.remove(cls, index)
	end

	return colors
end

-- point: Vector
-- triangle: array: A,B,C vectors
function IsPointBelongToTriangle(point, triangle)
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
