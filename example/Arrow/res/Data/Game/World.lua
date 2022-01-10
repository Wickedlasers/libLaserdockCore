include("Data/Game/Arrow.lua")
include("Data/Game/Map.lua")
include("Data/Game/CollisionHandler.lua")
include("Data/Game/DiamondSpawner.lua")
include("Data/Game/ParticleQuad.lua")

class "World" (Object)

function World:__init()
	Object.__init(self)

    self:AddAction(1.0, function ()
        self:Start()
    end)
end

function World:Start()
    Sound.Play(SFX.soundtrack)

    self:DisplayStartText()

    self.collected_diamonds_counter = 0

    self.map = Map()
    self:AddChild(self.map)

    self.arrow = Arrow(function (offset) -- OnMove
        self.map:Move(offset)
        self.diamond_spawner:UpdateSpawnInterval(offset)
    end)
    self:AddChild(self.arrow)
    self.arrow.pipe_update:Set(false)

    self.collision_handler =
        CollisionHandler(
            function () -- OnBorderCollision
                self.arrow.pipe_update:Set(false)
                self.diamond_spawner.pipe_render:Set(false)
                self.collision_handler = nil
                Sound.Stop(SFX.soundtrack)
                Sound.Play(SFX.collision)

                self:AddParticlesAnimation(self.arrow:GetHeadPosition(), function ()
                    self.arrow.pipe_render:Set(false)
                    self.map.pipe_render:Set(false)
                    self:DisplayResult(function ()
                        self:Restart()
                    end)
                end)
            end,
            function (diamond)
                Sound.Play(SFX.coin)
                self.collected_diamonds_counter = self.collected_diamonds_counter + 1
                if self.collected_diamonds_counter == 2 then
                    self.arrow:ChangeSpeedBy(1)
                elseif self.collected_diamonds_counter == 15 then
                    self.arrow:ChangeSpeedBy(1)
                elseif self.collected_diamonds_counter == 25 then
                    self.arrow:ChangeSpeedBy(1)
                elseif self.collected_diamonds_counter == 150 then
                    self.arrow:ChangeSpeedBy(1)
                elseif self.collected_diamonds_counter == 250 then
                    self.arrow:ChangeSpeedBy(1)
                end
                self.arrow:IncrementLength()
                self.diamond_spawner:RemoveChild(diamond)
            end)

    self.diamond_spawner = DiamondSpawner(function () -- GetBorderLines
        return self.map:GetBorderLineStrips()
    end)
    self:AddChild(self.diamond_spawner)
end

function World:Restart()
    self:RemoveChild(self.map)
    self.map = nil

    self:RemoveChild(self.arrow)
    self.arrow = nil

    self:RemoveChild(self.diamond_spawner)
    self.diamond_spawner = nil

    -- self.collision_handler = nil

    self:AddAction(0.1, function ()
        self:Start()
    end)
end

function World:OnUpdate(dt)
    if self.collision_handler then
        local left_border_points, right_border_points = self.map:GetBorderLineStrips()
        self.collision_handler:CheckHeadWithDiamond(self.arrow:GetPoints(), self.diamond_spawner:GetChildren())
        self.collision_handler:CheckHeadWithBorder(self.arrow:GetLines(), left_border_points, right_border_points)

        if InputEvents.LeftKeyPress or InputEvents.RightKeyPress then
            if self.start_text then
                self.arrow.pipe_update:Set(true)
                self:RemoveChild(self.start_text)
                self.start_text = nil
            end
        end
    end
end

function World:AddParticlesAnimation(position, OnFinish)
    local completed_particles_animation_counter = 0
    for i = 1, 6 do
        local random_position = Vector(math.random(1, 100), math.random(1, 100)) + position + Vector(-60, -60)
        local len = math.random(10, 50)
        local size = Vector(len, len)
        local result_color = Colorise.rgb2hex(GetRandomRGB())
        local quad = ParticleQuad(random_position, size)
        quad:SetColor(result_color)
        self:AddChild(quad)

        local quad_animation_component =
            TweenTemplates.ChangeValue(
                quad,
                3,
                size.x,
                math.random(1, 10) / 10, -- duration
                false,
                easing.inBounce,
                function (value)
                    quad:SetSize(Vector(value, value))
                end,
                function ()
                    self:RemoveChild(quad)
                    completed_particles_animation_counter = completed_particles_animation_counter + 1
                    if completed_particles_animation_counter == 6 then
                        OnFinish()
                    end
                end)
        quad:AddComponent("DecreaseAnimationComponent"..i, quad_animation_component)
    end
end

function World:DisplayResult(OnFinish)
    local text = TextLabel("SCORE: "..self.collected_diamonds_counter, 0.06, Vector(250, 550), Colors.CYAN)
    self:AddChild(text)
    self:AddAction(2.0, function ()
        self:RemoveChild(text)
        OnFinish()
    end)
end

function World:DisplayStartText()
    self.start_text = TextLabel("Press < or >", 0.05, Vector(200, 500), Colors.CYAN)
    self:AddChild(self.start_text)
end