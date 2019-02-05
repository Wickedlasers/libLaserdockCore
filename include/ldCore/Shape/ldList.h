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

#ifndef LDLIST_H
#define LDLIST_H

#include <iostream>

class ldList
{
    struct _list_node {
        _list_node* prev;
        _list_node* next;
        void* value;
    };

public:

    ldList();

    ~ldList();

    int length();

    bool push(void* value);

    void* pop();


    bool add(void* value);

    void add(ldList* list);

    bool insertAtIndex(int idx, void* value);

    void insertAtIndex(int idx, ldList* list);

    void removeLast();

    void removeByIndex(int idx);

    void removeByValue(void* value);

    void removeAll();

    void* getItemByIndex(int idx);

    bool containsObject(void* object);


    /**
     * Iterator methods
     */
public:

    void begin();

    void end();

    void* moveNext();

    bool isNext();

private:

    _list_node* m_pIterator = NULL;
    bool isForward = false;


private:

    _list_node* createNode(_list_node* prev, _list_node* next, void* value);

    _list_node* getNodeByIndex(int idx);

protected:

    int m_nLength;

    _list_node *m_pFront, *m_pTail;
};


#define listMakeObjectsPerformSelector(pList, func, elementType)      \
do {                                                                  \
    if(pList && pList->length() > 0)                                  \
    {                                                                 \
        pList->begin();                                               \
        while(pList->isNext())                                        \
        {                                                             \
            elementType pNode = (elementType) pList->moveNext();      \
            if(pNode)                                                 \
            {                                                         \
                pNode->func();                                        \
            }                                                         \
        }                                                             \
    }                                                                 \
}                                                                     \
while(false)


#define LDLIST_FOREACH(__array__, __object__) \
__array__->begin(); \
while(__array__->isNext() && (__object__ = __array__->moveNext()) != NULL)

#define LDLIST_FOREACH_REVERSE(__array__, __object__) \
__array__->end(); \
while(__array__->isNext() && (__object__ = __array__->moveNext()) != NULL)


#endif // LDLIST_H
