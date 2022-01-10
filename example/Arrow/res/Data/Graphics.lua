-- visualizer.m_renderer:begin(OL_LINESTRIP)
-- visualizer.m_renderer:vertex3(0.3, 0.3, 0, 0xFFFFFF, 1)
-- ...
-- visualizer.m_renderer:end_()

-- m_lua["OL_LINESTRIP"] = OL_LINESTRIP;
-- m_lua["OL_BEZIERSTRIP"] = OL_BEZIERSTRIP;
-- m_lua["OL_POINTS"] = OL_POINTS;

Graphics = {}

local MAX_VAL = 0.99

SCREEN_WIDTH = 800
SCREEN_HEIGHT = SCREEN_WIDTH / (1 / 1) -- ascpect ratio 1:1
HALF_SCREEN_WIDTH = SCREEN_WIDTH / 2
HALF_SCREEN_HEIGHT = SCREEN_HEIGHT / 2

local RATIO = 1 / (SCREEN_WIDTH / 2)

function SetScreenWidth(width)
    SCREEN_WIDTH = width
    RATIO = 1 / (SCREEN_WIDTH / 2)
end

local visualizer = nil

-- Конвертирует координаты с цетральной осью в верхнем левом углу в координаты с центральной осью по центру экрана
local function ConvertVector(v)
	local temp = (v * RATIO + Vector(-1 * MAX_VAL, -1 * MAX_VAL))
	temp.y = temp.y * -1
	return temp
end

function DivideLineOnSegments(start_position, finish_position, segments_amount)
	local result = {}
	local segment_length = (finish_position - start_position):GetLength() / segments_amount
	local one_vector = (finish_position - start_position):Normalize()
	local segment_vector = one_vector * segment_length

	table.insert(result, start_position)
	for i = 1, segments_amount do
		table.insert(result, segment_vector * i + start_position)
	end
	return result
end

function Graphics.init_visualizer(visualizer_)
	visualizer = visualizer_
end

function Graphics.render_linestrip(vectors, color, repeat_)
    repeat_ = repeat_ or 1
	color = color or Colors.RED
	visualizer.m_renderer:begin(OL_LINESTRIP)
	for i = 1, #vectors do
		local v = ConvertVector(vectors[i])
        for j = 1, repeat_ do
    		visualizer.m_renderer:vertex(v.x, v.y, color, 1)
    		visualizer.m_renderer:vertex(v.x, v.y, color, 1)
    		visualizer.m_renderer:vertex(v.x, v.y, color, 1)
        end
	end
	visualizer.m_renderer:end_()
end

-- color = ldColorUtil.lerpInt(Colors.RED, Colors.GREEN, t)
-- color = ldColorUtil.lerpInt(0xccccff, 0x0000FF, t)

-- text_label : обьет TextLabel
function Graphics.render_text(text_label)
	assert(text_label)
	text_label:innerDraw(visualizer.m_renderer)
end

function Graphics.render_points(vectors)
	visualizer.m_renderer:begin(OL_POINTS)
	for i = 1, #vectors do
		local v = ConvertVector(vectors[i])
		visualizer.m_renderer:vertex(v.x, v.y, Colors.RED, 1)
	end
	visualizer.m_renderer:end_()
end

function Graphics.render_gradient_circle(position_x, position_y, radius, segments_in_circle, color1, color2, angle, repeat_)
    angle = angle or 0
    repeat_ = repeat_ or 1

    local segment_angle = math.pi * 2 / segments_in_circle
    local vectors = {}

    for i = 1, segments_in_circle do
        local v = Vector()
        v.x = position_x + radius * math.cos(segment_angle * i + angle)
        v.y = position_y + radius * math.sin(segment_angle * i + angle)
        table.insert(vectors, v)
        if i == segments_in_circle and not half then
            local v = Vector()
            v.x = position_x + radius * math.cos(segment_angle + angle)
            v.y = position_y + radius * math.sin(segment_angle + angle)
            table.insert(vectors, v)
        end
    end

    local t = 1 / #vectors
    visualizer.m_renderer:begin(OL_LINESTRIP)
    for i = 1, #vectors do
        local new_color = ldColorUtil.lerpInt(color1, color2, t * (i - 1))

        local v = ConvertVector(vectors[i])
        for j = 1, repeat_ do
            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
        end
        if i == #vectors then
            new_color = ldColorUtil.lerpInt(color1, color2, t * #vectors)
            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
        end
    end
    visualizer.m_renderer:end_()
end

function Graphics.render_gradient_rectangle(position_x, position_y, width, height, color1, color2, repeat_value)
    repeat_value = repeat_value or 0

    local sub_vectors = {}
    table.insert(sub_vectors, Vector(position_x, position_y))
    table.insert(sub_vectors, Vector(position_x + width, position_y))
    table.insert(sub_vectors, Vector(position_x + width, position_y + height))
    table.insert(sub_vectors, Vector(position_x, position_y + height))
    table.insert(sub_vectors, Vector(position_x, position_y))

    visualizer.m_renderer:begin(OL_LINESTRIP)

    local colors = {
        {Colors.CYAN, Colors.MAGENTA},
        {Colors.MAGENTA, Colors.YELLOW},
        {Colors.YELLOW, Colors.CYAN},
        {Colors.RED, Colors.GREEN}
    }

    for i = 1, #sub_vectors - 1 do
        local vectors = DivideLineOnSegments(sub_vectors[i], sub_vectors[i + 1], 10)
        local t = 1 / #vectors

        color1 = colors[i][1]
        color2 = colors[i][2]

        for j = 1, #vectors do
            local v = ConvertVector(vectors[j])

            local new_color = ldColorUtil.lerpInt(color1, color2, t * (j - 1))

            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            for o = 1, repeat_value do
                visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            end
        end
    end

    visualizer.m_renderer:end_()

    -- local clrs = {Colors.CYAN, Colors.MAGENTA, Colors.YELLOW}
end

function Graphics.render_gradient_circle_by_color_scheme(position_x, position_y, radius, segments_in_circle, colors, angle, repeat_)
    angle = angle or 0
    repeat_ = repeat_ or 5

    local segment_angle = math.pi * 2 / segments_in_circle
    local vectors = {}
    for i = 1, segments_in_circle do
        local v = Vector()
        v.x = position_x + radius * math.cos(segment_angle * i + angle)
        v.y = position_y + radius * math.sin(segment_angle * i + angle)
        table.insert(vectors, v)
        if i == segments_in_circle then
            local v = Vector()
            v.x = position_x + radius * math.cos(segment_angle + angle)
            v.y = position_y + radius * math.sin(segment_angle + angle)
            table.insert(vectors, v)
        end
    end

    local t = 1 / #vectors
    visualizer.m_renderer:begin(OL_LINESTRIP)
    for i = 1, #vectors do
        local v = ConvertVector(vectors[i])
        for j = 1, repeat_ do
            visualizer.m_renderer:vertex(v.x, v.y, colors[i], 1)
        end
        if i == #vectors then
            -- colors[i] = ldColorUtil.lerpInt(color1, color2, t * #vectors)
            visualizer.m_renderer:vertex(v.x, v.y, colors[i], 1)
            visualizer.m_renderer:vertex(v.x, v.y, colors[i], 1)
            visualizer.m_renderer:vertex(v.x, v.y, colors[i], 1)
        end
    end
    visualizer.m_renderer:end_()
end

-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
function Graphics.render_circle(position_x, position_y, radius, segments_in_circle, color, angle, repeat_)
    angle = angle or 0
    color = color or Colors.RED
    repeat_ = repeat_ or 5
    local segment_angle = math.pi * 2 / segments_in_circle
    local vectors = {}
    for i = 1, segments_in_circle do
        local v = Vector()
        v.x = position_x + radius * math.cos(segment_angle * i + angle)
        v.y = position_y + radius * math.sin(segment_angle * i + angle)
        table.insert(vectors, v)
        if i == segments_in_circle then
            local v = Vector()
            v.x = position_x + radius * math.cos(segment_angle + angle)
            v.y = position_y + radius * math.sin(segment_angle + angle)
            table.insert(vectors, v)
        end
    end

    visualizer.m_renderer:begin(OL_LINESTRIP)
    for i = 1, #vectors do
        local v = ConvertVector(vectors[i])
        for j = 1, repeat_ do
            visualizer.m_renderer:vertex(v.x, v.y, color, 1)
        end
        if i == #vectors then
            visualizer.m_renderer:vertex(v.x, v.y, color, 1)
            visualizer.m_renderer:vertex(v.x, v.y, color, 1)
            visualizer.m_renderer:vertex(v.x, v.y, color, 1)
        end
    end
    visualizer.m_renderer:end_()
end

function Graphics.render_rectangle(position_x, position_y, width, height, color, repeat_value)
    color = color or Colors.RED
    repeat_value = repeat_value or 0

    local vectors = {}
    table.insert(vectors, Vector(position_x, position_y))
    table.insert(vectors, Vector(position_x + width, position_y))
    table.insert(vectors, Vector(position_x + width, position_y + height))
    table.insert(vectors, Vector(position_x, position_y + height))
    table.insert(vectors, Vector(position_x, position_y))

    visualizer.m_renderer:begin(OL_LINESTRIP)
    for i = 1, #vectors do
        local v = ConvertVector(vectors[i])
        visualizer.m_renderer:vertex(v.x, v.y, color, 1)
        for j = 1, repeat_value do
            visualizer.m_renderer:vertex(v.x, v.y, color, 1)
        end
    end
    visualizer.m_renderer:end_()
end

function Graphics.render_gradient_linestrip(vectors, colors_)
    visualizer.m_renderer:begin(OL_LINESTRIP)

    assert(#colors_ > 1)
    assert(#colors_ >= #vectors)

    local colors = {}
    table.insert(colors, {colors_[1], colors_[2]})
    for i = 2, #colors_ do
        table.insert(colors, {colors_[i - 1], colors_[i]})
    end

    for i = 1, #vectors - 1 do
        local points = DivideLineOnSegments(vectors[i], vectors[i + 1], 5)
        local t = 1 / #points

        color1 = colors[i][1]
        color2 = colors[i][2]

        for j = 1, #points do
            local v = ConvertVector(points[j])

            local new_color = ldColorUtil.lerpInt(color1, color2, t * (j - 1))

            visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            for k = 1, 1 do
                visualizer.m_renderer:vertex(v.x, v.y, new_color, 1)
            end
        end
    end

    visualizer.m_renderer:end_()
end