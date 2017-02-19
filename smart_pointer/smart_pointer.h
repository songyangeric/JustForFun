
#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

#include <map>

template <typename T>
class SmartPointer {
    public:
        SmartPointer(T *ptr);
        SmartPointer(const SmartPointer<T>&);
        SmartPointer<T>& operator=(const SmartPointer<T>&);
        T& operator*();
        T* operator->();
        bool operator==(const T*) const;
        bool operator==(const SmartPointer<T>&) const;
        bool operator!=(const T*) const;
        bool operator!=(const SmartPointer<T>&) const;
        ~SmartPointer();
    private:
        T *m_ptr;
        static std::map<T*, int> m_refMap;
};

template <typename T>
std::map<T*, int> SmartPointer<T>::m_refMap;

template <typename T>
SmartPointer<T>::SmartPointer(T *ptr) 
{
    m_ptr = ptr;
    if (m_refMap.find(m_ptr) == m_refMap.end()) {
        m_refMap[m_ptr] = 1;
    } else {
        m_refMap[m_ptr]++;
    }

    std::cout << "Pointer created at " << static_cast<const void*>(ptr) << std::endl; 
};

template <typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T>& other)
{
    m_ptr = other.m_ptr;
    if (m_refMap.find(m_ptr) == m_refMap.end()) {
        m_refMap[m_ptr] = 1;
    } else {
        m_refMap[m_ptr]++;
    }

    std::cout << "Pointer copied at " << static_cast<const void*>(other.m_ptr) << std::endl;
}

template <typename T>
SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer<T>& other)
{
    if (this == &other)
        return *this;
    
    if (m_refMap.find(m_ptr) == m_refMap.end()) 
        throw std::runtime_error("Error: element does not exist.");
    
    if (--m_refMap[m_ptr] == 0) {
        m_refMap.erase(m_ptr);
        delete m_ptr;
    }

    m_ptr = other.m_ptr;
    if (m_refMap.find(m_ptr) == m_refMap.end()) {
        m_refMap[m_ptr] = 1;
    } else {
        m_refMap[m_ptr]++;
    }

    std::cout << "Pointer assigned at " << static_cast<const void*>(other.m_ptr) << std::endl;

    return *this;
}

template <typename T>
SmartPointer<T>::~SmartPointer()
{
    if (m_refMap.find(m_ptr) == m_refMap.end()) 
        throw std::runtime_error("Error: element does not exist.");
    
    if (--m_refMap[m_ptr] == 0) {
        m_refMap.erase(m_ptr);
        delete m_ptr;
    }
    
    std::cout << "Pointer released at " << static_cast<const void*>(m_ptr) << std::endl;
}

template <typename T>
T& SmartPointer<T>::operator*()
{
    return *m_ptr;
}

template <typename T>
T* SmartPointer<T>::operator->()
{
    return m_ptr;
}

template <typename T>
bool SmartPointer<T>::operator==(const T* ptr) const
{
    return (ptr == m_ptr);
}

template <typename T>
bool SmartPointer<T>::operator==(const SmartPointer<T>& other) const
{
    return (other.m_ptr == m_ptr);
}

template <typename T>
bool SmartPointer<T>::operator!=(const T* ptr) const
{
    return (ptr != m_ptr);
}

template <typename T>
bool SmartPointer<T>::operator!=(const SmartPointer<T>& other) const
{
    return (other.ptr != m_ptr);
}

#endif
