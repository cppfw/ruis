#include "Updateable.hpp"


using namespace morda;



void Updateable::Updater::AddPending(){
	while(this->toAdd.size() != 0){
		UpdateQueue::T_Pair p;
		
		//TODO:
		
		this->toAdd.pop_front();
	}
}



ting::u32 Updateable::Updater::Update(){
	ting::u32 curTime = ting::timer::GetTicks();
	
	if(this->activeQueue->Size() == 0){
		//nothing to update
		
		//TODO: add pending
		
		return ting::u32(-1);
	}
	
	while(this->activeQueue->Size() != 0){
		if(this->activeQueue->Front().first > curTime){
			break;
		}
		ting::Ref<morda::Updateable> u = this->activeQueue->PopFront();
		if(!u){
			continue;
		}

		u->Update(curTime - u->startedAt);
		if(u->IsUpdating()){
			u->startedAt = curTime;
			this->toAdd.push_back(u);
		}
	}
	
	//TODO:
}
