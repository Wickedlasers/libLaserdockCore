#include "ldColormapManager.h"

#include <qdebug.h>

ldColormapManager::ldColormapManager()
{
}


ldColormapManager::~ldColormapManager()
{
}

bool ldColormapManager::loadColormap(QString filename, ldColormapManager::LOAD_STYLE loadStyle)
{
    bool result = false;
	
	ldColormap localAux;
	switch (loadStyle)	{
	
	case ldColormapManager::LOAD_STYLE::QT_RESOURCES:
		result = localAux.loadFromResourcesFile(filename);
		break;
	case ldColormapManager::LOAD_STYLE::LDCORE_RESOURCES:
		result = localAux.loadFromldCoreResourcesFile(filename);
		break;
	}

	if (result)	{
        m_loadedMaps.insert(localAux.name(), localAux);
	}

	return result;
}

bool ldColormapManager::existColormap(QString name)
{
	return m_loadedMaps.find(name) != m_loadedMaps.end();
}


int ldColormapManager::size() const
{
	return m_loadedMaps.size();
}

ldColormap ldColormapManager::getColormap(QString colormapName)
{
	return m_loadedMaps[colormapName];
}

void ldColormapManager::insert(ldColormap newColormap)
{
    m_loadedMaps.insert(newColormap.name(), newColormap);
}