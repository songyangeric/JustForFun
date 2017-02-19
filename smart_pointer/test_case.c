#include <iostream>
#include "smart_pointer.h"

using namespace std;


class SomeClass {
    public:
        SomeClass():m_ref(1) {};
        ~SomeClass() {};
        int get_ref() { return m_ref; }
    private:
        int m_ref;
};

void testcase()
{
    SmartPointer<SomeClass> ptr_class = new SomeClass();
    ptr_class = ptr_class;

    SmartPointer<SomeClass> ptr_class_copy = ptr_class;

    SmartPointer<SomeClass> ptr_class_assign = new SomeClass();
    ptr_class_assign = ptr_class;

    if (ptr_class == ptr_class_copy)
        cout << "Test ' == class_ptr' on copy constructor" << endl;

    if (ptr_class == ptr_class_assign)
        cout << "Test ' == class_ptr' on assign operator" << endl;

    cout << "Test '.' oprator: the reference count is "  << (*ptr_class).get_ref() << endl;
    cout << "Test '->' oprator: the reference count is "  << ptr_class->get_ref() << endl;
     
}

int main(void)
{
    testcase();

    return 0;
}
