#ifndef STATEMGR_H
#define STATEMGR_H

#include <vector>
#include <stdexcept>

enum StateMgrMode 
    {
    SINGLE_STATE,
    MULTI_STATE
    }; 

template <typename T>
struct StateMgr
    {
    // Create a new element and return it. This moves the new index to the newly inserted record.
    auto create () -> T&;
    // Returns the record at the current index.
    auto get () -> T&;
    // Moves the index by the specified amount. Throws an exception of (index + amt) outside of bounds.
    auto move (int amt) -> void;
    // Returns the current index.
    auto index () -> size_t;
    // Returns the number of managed states.
    auto size () -> size_t;

    // Returns true if the index is pointing to the last (latest) state 
    inline auto atEnd () -> bool { return m_index + 1 == m_states.size(); }
    // Returns true if the index is pointing to the first (earliest) state
    inline auto atBeg () -> bool { return m_index == 0; } 
private: 
    std::vector <T> m_states = {};
    size_t m_index = 0;
    };

template <typename T>
auto StateMgr<T>::create () -> T& 
    {
    if (size() == 0) 
        m_states.emplace_back ();
    else 
        {
        const T& val = m_states.back();
        m_states.emplace_back(val);
        }
    m_index = m_states.size() - 1;
    return m_states.back();
    }

template <typename T>
auto StateMgr<T>::get () -> T&
    {
    if (m_states.empty())
        throw std::runtime_error ( "container empty" );
    return m_states [ m_index ];
    }

template <typename T>
auto StateMgr<T>::move(int amt) -> void
    {
    const size_t new_index = m_index + amt;
    if (new_index >= size()) 
        throw std::runtime_error ( "specified move would go out of bounds for container" );
    m_index = new_index;
    }

template <typename T>
auto StateMgr<T>::index () -> size_t
    {
    return m_index;
    }

template <typename T>
auto StateMgr<T>::size () -> size_t
    {
    return m_states.size();
    }

#endif 