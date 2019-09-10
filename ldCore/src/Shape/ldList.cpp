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

#include <assert.h>
#include "ldCore/Shape/ldList.h"
#include <stdlib.h>

/*!
 * \class ldList
 * \inmodule Laserdock Project
 * \brief The ldList is a array class.
 */

/*!
 * \fn ldList::ldList()
 * \brief Creats an empty list.
 */
ldList::ldList()
: m_nLength(0)
, m_pFront(NULL)
, m_pTail(NULL)
{

}

/*!
 * \brief Destructor
 */
ldList::~ldList()
{
    if (m_nLength > 0) removeAll();
}

/*!
 * \fn int ldList::length()
 * \brief Gets the length of the list.
 */
int ldList::length() { return m_nLength; }

/*!
 * \fn bool ldList::push(void *value)
 * \brief This method pushs the \a value Object at tail of the list.
 * Returns the \c true if pushed successfully, otherwise \c false value.
 */
bool ldList::push(void *value)
{
    add(value); return true;
}

/*!
 * \fn void* ldList::pop()
 * \brief This method removes the last Object at the tail of the list and returns it.
 */
void* ldList::pop()
{
    int lastidx = length() - 1;

    if (lastidx < 0) return NULL;

    void* last = getItemByIndex(lastidx);

    removeByIndex(lastidx);

    return last;
}

/*!
 * \fn bool ldList::add(void *value)
 * \brief This method appends the specified object at the end of the list.
 * The parameter \a value is the pointer to object to add
 * Returns \c true if added succesfully, otherwise returns \c false.
 */
bool ldList::add(void *value)
{
    assert(value);

    _list_node* newnode = createNode(m_pTail, NULL, value);

    if (!newnode) return false;

    if (!m_pTail)
    {
        m_pFront = m_pTail = newnode;
    }
    else
    {
        m_pTail = newnode;
    }

    m_nLength++;

    return true;
}

/*!
 * \fn void ldList::add(ldList *list)
 * \brief This method appends all objects of the parameter \a list at the end of current list.
 * Returns the \c true if added successfully, otherwise \c false.
 */
void ldList::add(ldList *list)
{
    if (list == NULL || list->length() == 0) return;

    void* object;
    LDLIST_FOREACH(list, object)
    {
        add(object);
    }

}

/*!
 * \fn bool ldList::insertAtIndex(int idx, void *value)
 * \brief This method inserts specified \a value object at the position by specified \a idx parameter.
 * Returns the \c true if inserted successfully, otherwise \c false.
 */
bool ldList::insertAtIndex(int idx, void *value)
{
    assert(value);

    if (idx < 0 || idx > length()) return false;

    _list_node *prev = getNodeByIndex(idx - 1);
    _list_node *next = getNodeByIndex(idx);

    _list_node *newnode = createNode(prev, next, value);

    if (!newnode) return false;

    if (prev == NULL) m_pFront = newnode;

    if (next == NULL ) m_pTail = newnode;

    m_nLength++;

    return true;
}

/*!
 * \fn void ldList::insertAtIndex(int idx, ldList *list)
 * \brief This method inserts all objects of the specified \a list at the position  by specified \a idx parameter.
 * Returns the \c true if inserted successfully, otherwise \c false.
 */
void ldList::insertAtIndex(int idx, ldList *list)
{
    if (list == NULL || list->length() == 0) return;

    void* object;
    LDLIST_FOREACH(list, object)
    {
        insertAtIndex(idx++, object);
    }
}

/*!
 * \fn void ldList::removeLast()
 * \brief This method removes the last object.
 */
void ldList::removeLast()
{
    if (length() <= 0) return;

    _list_node* last = m_pTail->prev;

    free(m_pTail);

    m_pTail = last;

    m_pTail->next = NULL;

    m_nLength--;
}

/*!
 * \fn void ldList::removeByIndex(int idx)
 * \brief This method remove an object at the specifie \a idx position.
 */
void ldList::removeByIndex(int idx)
{
    if (idx < 0 || idx > length() - 1) return;

    _list_node * node = getNodeByIndex(idx);

    _list_node *prev = node->prev;
    _list_node *next = node->next;

    if (node == m_pFront)
        m_pFront = next;
    else
        prev->next = next;

    if (node == m_pTail)
        m_pTail = prev;
    else
        next->prev = prev;

    free(node);

    m_nLength--;
}

/*!
 * \fn void ldList::removeByValue(void* value)
 * \brief This method removes an object that has the specified \a value.
 * If there are some object that has specified \a value, the first object among them will be removed.
 */
void ldList::removeByValue(void* value)
{
    // valuse must be not null;
    if (value == NULL) return;

    _list_node * node = NULL;

    _list_node* inode = m_pFront;

    while (inode)
    {
        if (inode->value == value) {
            node = inode;
        }

        inode = inode->next;
    }


    // can't find value node. so will return;
    if (node == NULL) return;

    _list_node *prev = node->prev;
    _list_node *next = node->next;

    if (node == m_pFront)
        m_pFront = next;
    else
        prev->next = next;

    if (node == m_pTail)
        m_pTail = prev;
    else
        next->prev = prev;

    free(node);

    m_nLength--;
}

/*!
 * \fn void ldList::removeAll()
 * \brief This method removes all object in the list.
 */
void ldList::removeAll()
{
    _list_node* node = m_pFront;
    _list_node* tmp;

    while (node)
    {
        tmp = node->next;

        free(node);

        node = tmp;
    }

    m_pFront = m_pTail = NULL;

    m_nLength = 0;
}

/*!
 * \fn void* ldList::getItemByIndex(int idx)
 * \brief This method returns an object at the specified \a idx position.
 */
void* ldList::getItemByIndex(int idx)
{
    _list_node* node = getNodeByIndex(idx);

    if (!node) return NULL;

    return node->value;
}

/*!
 * \fn bool ldList::containsObject(void* object)
 * \brief Checks whether the list contains the specified \a object.
 * If contains, returns \c true value, otherwise returns \c false value.
 */
bool ldList::containsObject(void* object)
{
    if (object == NULL) return false;

    _list_node* node = m_pFront;

    while (node)
    {
        if (node->value == object) return true;

        node = node->next;
    }

    return false;
}

ldList::_list_node* ldList::createNode(ldList::_list_node *prev, ldList::_list_node *next, void *value)
{
    _list_node* node = (_list_node*)malloc(sizeof(_list_node));

    if (!node) return NULL;

    if (prev) prev->next = node;

    if (next) next->prev = node;

    node->prev = prev;
    node->next = next;
    node->value = value;

    return node;
}

ldList::_list_node* ldList::getNodeByIndex(int idx)
{
    if (idx < 0) return NULL;

    _list_node* node = m_pFront;

    while (node)
    {
        if (idx == 0) return node;

        node = node->next;
        idx--;
    }

    return NULL;
}


/// Iterator methods

/*!
 * \fn void ldList::begin()
 * \brief Begins the iterator
 * \ingroup Iterator
 */
void ldList::begin()
{
    m_pIterator = m_pFront;
    isForward = true;
}

/*!
 * \fn void ldList::end()
 * \brief Ends the interator
 * \ingroup Iterator
 */
void ldList::end()
{
    m_pIterator = m_pTail;
    isForward = false;
}

/*!
 * \fn void* ldList::moveNext()
 * \brief Returns current object and move the iterator forward by one step.
 * \ingroup Iterator
 */
void* ldList::moveNext()
{
    if (!m_pIterator) return NULL;

    void* value = m_pIterator->value;

    if (isForward)
        m_pIterator = m_pIterator->next;
    else
        m_pIterator = m_pIterator->prev;

    return value;
}

/*!
 * \fn bool ldList::isNext()
 * \brief Check whether the iterator can move forward.
 * Returns the \c true if the iterator can move, else, \c false if the iterator reached to the end of list.
 * \ingroup Iterator
 */
bool ldList::isNext()
{
    return m_pIterator != NULL;
}
