TweenTemplates = {}

TweenTemplates.ChangePosition =
	function (object, from, to, duration, cyclic, Easing, OnFinish)
		local Setter = function(x, y)
			object:SetPosition(Vector(x, y))
		end
		return Tween({from.x, from.y}, {to.x, to.y}, duration, cyclic, Easing, Setter, OnFinish)
	end

TweenTemplates.ChangeValue =
	function (object, from, to, duration, cyclic, Easing, OnChangeValue, OnFinish)
		return Tween({from}, {to}, duration, cyclic, Easing, OnChangeValue, OnFinish)
	end

TweenTemplates.ChangeSize =
	function (object, from, to, duration, cyclic, Easing, OnFinish)
		local Setter = function(x, y)
			object:SetSize(Vector(x, y))
		end
		return Tween({from.x, from.y}, {to.x, to.y}, duration, cyclic, Easing, Setter, OnFinish)
	end