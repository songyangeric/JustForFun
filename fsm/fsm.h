#ifndef __FSM_H__
#define __FSM_H__

#include <vector>
#include <map>
#include <queue>

using std::queue;
using std::map;
using std::vector;

typedef struct fsm_transition_s {
    int event_id;
    int nxt_state;
    int action_id;
}fsm_trans_t;

// transitions bound to a state
typedef struct fsm_state_tran_s {
    vector<fsm_trans_t> vec_transitions;
}fsm_state_tran_t;

class FSMState {
    public:
        FSMState();
        virtual ~FSMState();
        // add all the transitions bound to a state
        int add_fsm_config(int cur_state, int event_id, int nxt_state, int action_id);
        // inititate the state
        int init_fsm_state(int state_id);
        // change state based on event 
        int exec_fsm_event(int event_id);

        int get_fsm_cur_state() {
            return cur_state;
        }

    protected:
        // detailed action must be implemented by the child 
        virtual int exec_fsm_action(int action_id) = 0;
        
    private:
        int set_fsm_cur_state(int state_id) {
            cur_state = state_id;
        }
        
        // get the next state and action based on the current state and incoming event
        int get_action_id(int cur_state, int event_id, int& nxt_state, int& action_id); 

    private:
        int cur_state; 
        map<int, fsm_state_tran_t> map_states; // collection of transitions for a state
};

#endif
