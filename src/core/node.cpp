#include "core/node.hpp"


void Node::preCycle(){

}

void Node::handleMessage(Message&& msg){

}

void Node::postCycle(){
    message_box.changePhase();
}