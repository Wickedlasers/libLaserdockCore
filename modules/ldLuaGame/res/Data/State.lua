class "State"

function State:__init()
    self.enabled = true
end

function State:Set(enabled)
    self.enabled = enabled
end

function State:Get()
    return self.enabled
end