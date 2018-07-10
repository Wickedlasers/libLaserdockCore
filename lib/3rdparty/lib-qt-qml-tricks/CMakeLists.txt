set(LIBQQMLHELPERS_SOURCES
    src/QtLibrary.qbs
    src/qqmlgadgetlistmodel.cpp
    src/qqmlgadgetlistmodel.h
    src/qqmlhelpers.cpp
    src/qqmlhelpers.h
    src/qqmlmodels.h
    src/qqmlobjectlistmodel.cpp
    src/qqmlobjectlistmodel.h
    src/qqmlsvgiconhelper.cpp
    src/qqmlsvgiconhelper.h
    src/qqmlvariantlistmodel.cpp
    src/qqmlvariantlistmodel.h
    src/qqmlvariantlistmodel_p.h
    src/qquickpolygon.cpp
    src/qquickpolygon.h
    src/qtbitstream.h
    src/qtcobs.h
    src/qtjsonpath.h    )

# library
add_library(libqtqmltricks ${LIBQQMLHELPERS_SOURCES})

