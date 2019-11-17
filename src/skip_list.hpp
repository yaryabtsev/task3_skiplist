////////////////////////////////////////////////////////////////////////////////
// Module Name:  skip_list.h/hpp
// Authors:      Leonid Dworzanski, Sergey Shershakov
// Version:      2.0.0
// Date:         28.10.2018
//
// This is a part of the course "Algorithms and Data Structures"
// provided by  the School of Software Engineering of the Faculty
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

// !!! DO NOT include skip_list.h here, 'cause it leads to circular refs. !!!

#include <cstdlib>
#include "skip_list.h"
#include <random>

//==============================================================================
// class NodeSkipList
//==============================================================================

template <class Value, class Key, int numLevels>
void NodeSkipList<Value, Key, numLevels>::clear(void)
{
    for (int i = 0; i < numLevels; ++i)
        Base::nextJump[i] = 0;

    Base::levelHighest = -1;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
    clear();
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey)
{
    clear();

    Base::Base::key = tkey;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey, const Value& val)
{
    clear();

    Base::Base::key = tkey;
    Base::Base::value = val;
}


//==============================================================================
// class SkipList
//==============================================================================

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
    _probability = probability;

    // Lets use m_pPreHead as a final sentinel element
    for (int i = 0; i < numLevels; ++i)
        Base::_preHead->nextJump[i] = Base::_preHead;

    Base::_preHead->levelHighest = numLevels - 1;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::~SkipList() = default;
/*{
    Node* x = Base::_preHead;
    while (x->next != Base::_preHead)
        removeNext(x);
    delete x;
}*/


//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
NodeSkipList<Value,Key, numLevels>* SkipList<Value, Key, numLevels>::findLastLessThan(const Key& key) const
{
    Node* x = Base::_preHead->nextJump[numLevels-1];
    for(int i = numLevels-1; i > -1; --i)
        while (x->nextJump[i]->key < key && x->nextJump[i] != Base::_preHead)
            x = x->nextJump[i];
    while (x->next->key < key && x->next != Base::_preHead)
        x = x->next;
    return x;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
NodeSkipList<Value,Key, numLevels>* SkipList<Value, Key, numLevels>::findFirst(const Key& key) const
{
    Node* x = findLastLessThan(key)->next;
    if(x->key != key)
        return nullptr;
    return x;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(Node* nodeBefore)
{
    if (nodeBefore == nullptr
        || nodeBefore->nextJump[0] == nullptr
        || nodeBefore->nextJump[0] == Base::_preHead)
    {
        throw std::invalid_argument("SkipList error, invalid previous node");
    }
    NodeSkipList<Value,Key, numLevels>* x = Base::_preHead->nextJump[numLevels-1];
    for(int i = numLevels - 1; i > -1; --i) {
        while (x->nextJump[i]->key < nodeBefore->next->key && x->nextJump[i] != Base::_preHead)
            x = x->nextJump[i];
        while (x->nextJump[i] != nodeBefore->next && i <= nodeBefore->next->levelHighest)
            x = x->nextJump[i];
        if (x->nextJump[i] == nodeBefore->next && i <= nodeBefore->next->levelHighest)
            x->nextJump[i] = nodeBefore->next->nextJump[i];
    }
    nodeBefore->next = nodeBefore->next->next;
    delete x;
}

//------------------------------------------------------------------------------

/*int randomLevel(double p)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int newLevel = 1;
    while(std::generate_canonical<double, 10>(gen) < p)
        ++newLevel;
    return newLevel;
}
int randomLevel1(double p)
{
    srand(time(0));
    int newLevel = 1;
    while(double(std::abs(rand())) / RAND_MAX <  p)
        ++newLevel;
    return newLevel;
}*/

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(const Value& val, const Key& key)
{
    //int levelHighest = randomLevel(_probability);
    srand(time(0));
    int levelHighest = 0;
    while(double(std::abs(rand())) / RAND_MAX <  _probability)
        ++levelHighest;

    if(levelHighest > numLevels)
        levelHighest = numLevels;

    //Node* newNode = new Node(key, val);
    //(newNode->levelHighest) = levelHighest - 1;
    Node* newNode = new Node;
    newNode->levelHighest = levelHighest - 1;
    newNode->value = val;
    newNode->key = key;

    Node* x = Base::_preHead->nextJump[numLevels - 1];
    for(int i = numLevels - 1; i > -1; --i) {
        while (x->nextJump[i]->key <= key && x->nextJump[i] != Base::_preHead)
            x = x->nextJump[i];
        if(i <= levelHighest){
            newNode->nextJump[i] = x->nextJump[i];
            x->nextJump[i] = newNode;
        }
    }
    while (x->next->key <= key && x->next != Base::_preHead)
        x = x->next;
    newNode->next = x->next;
    x->next = newNode;
}