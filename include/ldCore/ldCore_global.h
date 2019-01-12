/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef LDCORE_GLOBAL_H
#define LDCORE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QVariant>

/** Shared library export macro */
#ifdef Q_OS_WIN

#if defined(LDCORE_LIBRARY)
#  define LDCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LDCORESHARED_EXPORT Q_DECL_IMPORT
#endif // LDCORE_LIBRARY

#else // Q_OS_WIN
# define LDCORESHARED_EXPORT
#endif // Q_OS_WIN

#endif // LDCORE_GLOBAL_H
