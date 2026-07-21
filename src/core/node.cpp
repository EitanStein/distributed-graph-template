#include "core/node.hpp"


void Node::preCycle(){

}

void Node::handleMessage(Message&& msg){

}

void Node::postCycle(){
    message_box.changePhase();
}

void Node::Task::operator()(){
    // TODO only single task right now - change to allow more tasks or keep as is?
    switch(task){
    case Task::Name::Cycle:
        node.get().cycle();
        break;
    default:
        LOG_ERROR("unknown task");
    }

}