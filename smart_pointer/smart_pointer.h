
#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

template <typename T>
class SmartPointer {
    public:
        SmartPointer() : m_ptr(NULL) { std::cout << "Unknown pointer created." << std::endl;};
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
};

template <typename T>
SmartPointer<T>::SmartPointer(T *ptr) 
{
    m_ptr = ptr;
    if (m_ptr)
        m_ptr->inc_ref_count();

    std::cout << "Pointer created at " << static_cast<const void*>(ptr) << std::endl; 
};

template <typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T>& other)
{
    m_ptr = other.m_ptr;
    if (m_ptr)
        m_ptr->inc_ref_count();

    std::cout << "Pointer copied at " << static_cast<const void*>(other.m_ptr) << std::endl;
}

template <typename T>
SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer<T>& other)
{
    if (this == &other)
        return *this;
    
    if (m_ptr && (m_ptr->dec_ref_count() == 0))
        delete m_ptr;

    m_ptr = other.m_ptr;
    if (m_ptr)
        m_ptr->inc_ref_count();

    std::cout << "Pointer assigned at " << static_cast<const void*>(other.m_ptr) << std::endl;

    return *this;
}

template <typename T>
SmartPointer<T>::~SmartPointer()
{
    std::cout << "Pointer released at " << static_cast<const void*>(m_ptr) << std::endl;
    if (m_ptr && (m_ptr->dec_ref_count() == 0))
        delete m_ptr;
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
