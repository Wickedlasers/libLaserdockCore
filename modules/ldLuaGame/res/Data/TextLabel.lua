class "TextLabel" (Object)

local RATIO = 1 / (SCREEN_WIDTH)
local MAX_VAL = 0.99
-- Конвертирует координаты с цетральной осью в верхнем левом углу в координаты с центральной осью в левом нижнем углу
local function Convert(v)
	local temp = (v * RATIO + Vector(0, -MAX_VAL))
	temp.y = temp.y * -1
	return temp
end

function TextLabel:__init(txt, size, position, color)
	Object.__init(self)

	self.txt = txt or "Message"
	local result_position = Convert(position)
	self._position = ldVec2.new(result_position.x, result_position.y)
	self.size = size or 0.1
	self.color = color or Colors.GREEN
	self.text_label = ldTextLabel.new(QString.fromStdString(self.txt), self.size, self._position)
	self.text_label:setColor(self.color)
end

function TextLabel:OnDraw()
	Graphics.render_text(self.text_label)
end

-- position : Vector
-- self.position : Vec2
function TextLabel:SetPosition(position)
	local result_position = Convert(position)
	self._position.x = result_position.x
	self._position.y = result_position.y
end

function TextLabel:GetPosition()
	return self._position
end

function TextLabel:SetSize(size)
	assert(size > 0 and size <= 1.0)
	self.size = size

	self.text_label:setSize(self.size)
end

function TextLabel:GetSize()
	return self.size
end

function TextLabel:SetColor(color)
	self.color = color

	self.text_label:setColor(color)
end

function TextLabel:SetText(txt)
	assert(#txt > 0)
	self.txt = txt

	self.text_label:setText(QString.fromStdString(self.txt))
end

function TextLabel:GetText()
	return self.txt
end