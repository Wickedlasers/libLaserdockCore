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

#ifndef LDENUMHELPER_H
#define LDENUMHELPER_H

#include <iostream>

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "ldCore/ldCore_global.h"

namespace ldEnumHelper
{
    /// 'class enum' value from C++11 to int
    template <typename Enumeration>
    auto as_integer(Enumeration const value)
        -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    /// hash function for 'enum class' from C++11
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };

    /// class Enum allows to iterate over all "Class enum"
    /// just define MyEnum::First and MyEnum::Last in your enum
    ///     for(const auto &myEnum : ldEnumHelper::Enum<MyEnum>()) {
    ///         ...
    ///     }
    template< typename T >
    class Enum
    {
    public:
       class Iterator
       {
       public:
          Iterator( int value ) :
             m_value( value )
          { }

          T operator*( void ) const
          {
             return (T)m_value;
          }

          void operator++( void )
          {
             ++m_value;
          }

          bool operator!=( Iterator rhs )
          {
             return m_value != rhs.m_value;
          }

       private:
          int m_value;
       };

    };

    template< typename T >
    typename Enum<T>::Iterator begin( Enum<T> )
    {
       return typename Enum<T>::Iterator( (int)T::First );
    }

    template< typename T >
    typename Enum<T>::Iterator end( Enum<T> )
    {
       return typename Enum<T>::Iterator( ((int)T::Last) + 1 );
    }


    /// This class allows to iterate over all "Class enum" with flag values. each value is twice more than previous
    /// just define MyEnum::First and MyEnum::Last in your enum
    ///     for(const auto &myEnum : ldEnumHelper::Enum_Flag<MyEnum>()) {
    ///         ...
    ///     }
    template< typename T >
    class Enum_Flag
    {
    public:
       class Iterator
       {
       public:
          Iterator( int value ) :
             m_value( value )
          { }

          T operator*( void ) const
          {
             return (T)m_value;
          }

          void operator++( void )
          {
             m_value *= 2;
          }

          bool operator!=( Iterator rhs )
          {
             return m_value != rhs.m_value;
          }

       private:
          int m_value;
       };

    };

    template< typename T >
    typename Enum_Flag<T>::Iterator begin( Enum_Flag<T> )
    {
       return typename Enum_Flag<T>::Iterator( (int)T::First );
    }

    template< typename T >
    typename Enum_Flag<T>::Iterator end( Enum_Flag<T> )
    {
       return typename Enum_Flag<T>::Iterator( ((int)T::Last) * 2);
    }


}

#endif // LDENUMHELPER_H
