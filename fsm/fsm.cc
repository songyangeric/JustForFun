#include "fsm.h"

using std::pair;

FSMState::FSMState()
{
    map_states.clear();
}

FSMState::~FSMState() 
{
}

int FSMState::init_fsm_state(int state_id)
{
    map<int, fsm_state_tran_t>::iterator state_iter;
    state_iter = map_states.find(state_id);
    if (state_iter != map_states.end()) {
        fsm_state_tran_t state = state_iter->second;
        if (state.vec_transitions.size() == 0) {
            // No transition for this state
            return -2; 
        }
    } else {
        // No such state exists. Please check
        return -1; 
    }

    set_fsm_cur_state(state_id);
}

int FSMState::add_fsm_config(int cur_state, int event_id, int nxt_state, int action_id)
{
    fsm_trans_t transition;
    transition.event_id = event_id;
    transition.nxt_state = nxt_state;
    transition.action_id = action_id;

    map<int, fsm_state_tran_t>::iterator state_iter;
    state_iter = map_states.find(cur_state);
    if (state_iter != map_states.end()) {
        state_iter->second.vec_transitions.push_back(transition);
    } else {
        fsm_state_tran_t state;
        state.vec_transitions.push_back(transition);
        map_states.insert(pair<int, fsm_state_tran_t>(cur_state, state));
    }    

//    for (state_iter = map_states.begin(); state_iter != map_states.end(); ++state_iter) {
//        fsm_state_tran_t state = state_iter->second;
//        for (int i = 0; i < state.vec_transitions.size(); ++i) {
//            transition = state.vec_transitions[i];
//        }
//    }
}

int FSMState::exec_fsm_event(int event_id)
{
    int cur_state = get_fsm_cur_state();

    int action_id;
    int nxt_state;
    int ret = get_action_id(cur_state, event_id, nxt_state, action_id);
    if (ret == 0) {
        set_fsm_cur_state(nxt_state);
        exec_fsm_action(nxt_state);
    }

    return -1;
}

int FSMState::get_action_id(int cur_state, int event_id, int& nxt_state, int& action_id)
{
    map<int, fsm_state_tran_t>::iterator state_iter;
    state_iter = map_states.find(cur_state);
    if (state_iter != map_states.end()) {
        fsm_state_tran_t state = state_iter->second;
        vector<fsm_trans_t>::iterator tran_iter;
        for (tran_iter = state.vec_transitions.begin(); tran_iter != state.vec_transitions.end(); ++tran_iter) {
            if (tran_iter->event_id == event_id) {
                nxt_state = tran_iter->nxt_state;
                action_id = tran_iter->action_id;

                return 0;
            }
        }
        // No transition for this state.
        return -2;
    } else {
        // No such state exists.
        return -1;
    }
}
