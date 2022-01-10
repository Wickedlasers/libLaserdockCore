Sound = {}

Sound.visualizer = nil

Sound.init_visualizer = function(visualizer_)
	Sound.visualizer = visualizer_
end

Sound.Add = function(id, subpath)
   	Sound.visualizer.m_soundEffects:insert(
        id,
        QString.fromStdString(ldCore.instance():resourceDir():toStdString().."/sound/"..subpath))
end

Sound.Play = function(id)
    Sound.visualizer.m_soundEffects:play(id)
end

Sound.Stop = function(id)
	Sound.visualizer.m_soundEffects:stop(id)
end

Sound.SetLoops = function(id, loops)
        Sound.visualizer.m_soundEffects:setLoops(id, loops)
end

-- "setLoops", &ldSoundEffects::setLoops,
-- "setSoundEnabled", &ldSoundEffects::setSoundEnabled,
-- "setSoundLevel", &ldSoundEffects::setSoundLevel);
