#include <iostream>
#include <unistd.h>
#include "fsm.h"

using std::cout;
using std::endl;

enum DoorState {
    S_OPENED,
    S_CLOSED,
    S_LOCKED,
    S_UNLOCKED,
};

enum DoorEvent {
    E_OPEN,
    E_CLOSE,
    E_LOCK,
    E_UNLOCK,
};

enum DoorAction {
    A_OPEN,
    A_CLOSE,
    A_LOCK,
    A_UNLOCK,
};

class DoorTest : public FSMState
{
    public:
        int init_fsm(int init_state);

    protected:
        virtual int exec_fsm_action(int action_id);

    private:
        void open_door() {
            cout << "OPEN THE DOOR" << endl;
        }

        void close_door() {
            cout << "CLOSE THE DOOR" << endl;
        }

        void lock_door() {
            cout << "LOCK THE DOOR" << endl;
        }

        void unlock_door() {
            cout << "UNLOCK THE DOOR" << endl;
        }
};

int DoorTest::init_fsm(int init_state)
{
    // OPENED --> CLOSED
    add_fsm_config(S_OPENED, E_CLOSE, S_CLOSED, A_CLOSE);
    // CLOSED --> OPENED, CLOSED --> LOCKED
    add_fsm_config(S_CLOSED, E_OPEN, S_OPENED, A_OPEN);
    add_fsm_config(S_CLOSED, E_LOCK, S_LOCKED, A_LOCK);
    // LOCKED --> UNLOCKED
    add_fsm_config(S_LOCKED, E_UNLOCK, S_UNLOCKED, A_UNLOCK);
    // UNLOCKED --> OPENED, UNLOCKED --> LOCKED
    add_fsm_config(S_UNLOCKED, E_OPEN, S_OPENED, A_OPEN);
    add_fsm_config(S_UNLOCKED, E_LOCK, S_LOCKED, A_LOCK);

    init_fsm_state(init_state);

    return 0;
}

int DoorTest::exec_fsm_action(int action_id)
{
    switch (action_id) {
        case A_OPEN:
            open_door();
            break;
        case A_CLOSE:
            close_door();
            break;
        case A_LOCK:
            lock_door();
            break;
       case A_UNLOCK:
            unlock_door();
            break;
       default:
            cout << "UNKNOWN ACTION" << endl;
            break;
    }
 
    return 0;
}

int main(int argc, char* argv[])
{
    DoorTest *door_test = new DoorTest();
    
    // start test
//     door_test->init_fsm(S_OPENED);
//     door_test->exec_fsm_event(E_CLOSE);
//     door_test->exec_fsm_event(E_LOCK);
//     door_test->exec_fsm_event(E_UNLOCK);
//     door_test->exec_fsm_event(E_OPEN);
//     // invalid transition from OPENED to LOCKED
//     door_test->exec_fsm_event(E_LOCK);
     door_test->init_fsm(S_LOCKED);
     // invalid transition from LOCKED to CLOSED
     door_test->exec_fsm_event(E_CLOSE);
     // no actual transition
     door_test->exec_fsm_event(E_LOCK);
     door_test->exec_fsm_event(E_UNLOCK);
     door_test->exec_fsm_event(E_OPEN);
     // invalid transition from OPENED to LOCKED
     door_test->exec_fsm_event(E_LOCK);
   
    delete door_test;

    return 0;
}
