function extends (B, A) -- B->A: B наследует/расширяет A
    setmetatable(B, {
      __index = A, -- this is what makes the inheritance work
      __call = function (cls, ...)
        local self = setmetatable({}, {__index = cls})
        self:__init(...)
        return self
      end
    })
end

function class(name)
    _G[name] = {}
    _G[name].__index = _G[name]

    setmetatable(_G[name], {
        __call = function (cls, ...)
            local self = setmetatable({}, cls)
            self:__init(...)
            return self
        end
    })

    return function (B) -- опциональный параметр, если он есть то наследуемся
        if B then
            extends(_G[name], B)
        end
    end
end

function implements(...)
    local interfaces = {...}
    return function (implementation_class_name)
        for i = 1, #interfaces do
            for k, v in pairs(interfaces[i]) do
                if k ~= "__init" and k ~= "__index" then
                    _G[implementation_class_name][k] = function ()
                        assert(false, "Method "..implementation_class_name..":"..k.." did not implemented!")
                    end
                end
            end
        end
    end
end