class "Children"

function Children:__init()
    self.children = {}
end

function Children:ForEach(Action)
    for name, child in pairs(self.children) do
        if child ~= 0 then
            Action(child)
        end
    end
end

function Children:Add(name, child)
    assert(not self.children[name] or self.children[name] == 0)
    self.children[name] = child
end

function Children:Remove(name)
    assert(self.children[name])
    self.children[name] = 0
end

function Children:Get(name)
    if self.children[name] and self.children[name] ~= 0 then
        return self.children[name]
    else
        return nil
    end
end

class "Object"

function Object:__init()
    self.name = nil
    self.child_index = 0
    self.action_index = 0
    self.children = Children()
    self.components = {}

    self.parent = nil

    self.queue_free_components = {}

    self.pipe_update = State()
    self.pipe_render = State()
end

function Object:SetName(name)
    self.name = name
end

function Object:GetName()
    return self.name
end

function Object:_FreeComponents()
    for name, _ in pairs(self.queue_free_components) do
        self.components[name] = nil
    end
    self.queue_free_components = {}
end

function Object:Update(delta)
    if self.pipe_update:Get() then
        for name, component in pairs(self.components) do
            if component.Update then
                component:Update(delta)
            end
        end
        self.children:ForEach(function (child)
            child:Update(delta)
        end)
        if self.OnUpdate then
            self:OnUpdate(delta)
        end

        -- В конце апдейта удаляем все компоненты которые были добавлены в очередь на удаление
        self:_FreeComponents()
    end
end

function Object:Draw()
    if self.pipe_render:Get() then
        self.children:ForEach(function (child)
            child:Draw()
        end)
        if self.OnDraw then
            self:OnDraw()
        end
    end
end

function Object:AddChild(object)
    object:SetParent(self)
    if not object:GetName() then
        self.child_index = self.child_index + 1
        local name = "Obj"..self.child_index
        object:SetName(name)
        self.children:Add(name, object)
    else
        self.children:Add(object:GetName(), object)
    end
end

function Object:RemoveChild(child)
    self.children:Remove(child:GetName())
end

function Object:RemoveChildren()
    self.children:ForEach(function (child)
        self:RemoveChild(child)
    end)
end

function Object:GetChild(name)
    return self.children:Get(name)
end

function Object:GetChildren()
    return self.children
end

function Object:AddComponent(name, component)
    assert(not self:HasComponent(name), name)
    self.components[name] = component
    return name, component
end

function Object:HasComponent(name)
    return self.components[name] or true and false
end

-- компонент удаляется не сразу, а в конце апдейта на котором он был удален
-- это необходимо так как нельзя удалять элемент таблицы во время прохода по самой таблице
function Object:RemoveComponent(name)
    assert(self:HasComponent(name))
    self.queue_free_components[name] = true
end

function Object:AddAction(delay, Action)
    self.action_index = self.action_index + 1
    local name = "Timer"..tostring(self.action_index)
    local timer = Timer(delay, function ()
        self:RemoveComponent(name)
        Action()
    end)
    self:AddComponent(name, timer)
end

function Object:SetParent(parent)
    self.parent = parent
end

function Object:GetParent()
    return self.parent
end