#include "Updateable.hpp"

#include "App.hpp"

#include <ting/timer.hpp>
#include <zlib.h>



using namespace morda;



void Updateable::Updater::UpdateQueue::Insert(const T_Pair& p){
	if(this->l.size() == 0){
		this->l.push_back(p);
		return;
	}
	
	//quick check
	if(this->l.back().first <= p.first){
		this->l.push_back(p);
		return;
	}
	
	//otherwise, go from the beginning
	for(T_List::iterator i = this->l.begin(); i != this->l.end(); ++i){
		if(i->first >= p.first){
			this->l.insert(i, p); //inserts before iterator
			return;
		}
	}
	
	ASSERT(false)
}



void Updateable::Updater::AddPending(){
	while(this->toAdd.size() != 0){
		UpdateQueue::T_Pair p;
		
		p.first = this->toAdd.front()->EndAt();
		p.second = this->toAdd.front();
		
		this->toAdd.pop_front();
		
		if(p.first < this->lastUpdatedTimestamp){
			TRACE(<< "Updateable::Updater::AddPending(): inserted to inactive queue" << std::endl)
			this->inactiveQueue->Insert(p);
		}else{
			TRACE(<< "Updateable::Updater::AddPending(): inserted to active queue" << std::endl)
			this->activeQueue->Insert(p);
		}
	}
}



void Updateable::Updater::UpdateUpdateable(const ting::Ref<morda::Updateable>& u){
	//if weak ref gave invalid strong ref
	if(!u){
		return;
	}

	if(u->IsUpdating()){
		u->Update(this->lastUpdatedTimestamp - u->startedAt);
		if(u->IsUpdating()){
			u->startedAt = this->lastUpdatedTimestamp;
			this->toAdd.push_back(u);
		}
	}
}



ting::u32 Updateable::Updater::Update(){
	ting::u32 curTime = ting::timer::GetTicks();
	
	TRACE(<< "Updateable::Updater::Update(): invoked" << std::endl)
	
	//check if there is a warp around
	if(curTime < this->lastUpdatedTimestamp){
		this->lastUpdatedTimestamp = curTime;
		
		TRACE(<< "Updateable::Updater::Update(): time has warped, this->activeQueue->Size() = " << this->activeQueue->Size() << std::endl)
		
		//if time has warped, then all Updateables from active queue have expired.
		while(this->activeQueue->Size() != 0){
			this->UpdateUpdateable(this->activeQueue->PopFront());
		}
		
		std::swap(this->activeQueue, this->inactiveQueue);
	}else{
		this->lastUpdatedTimestamp = curTime;
	}
	ASSERT(this->lastUpdatedTimestamp == curTime)
	
	TRACE(<< "Updateable::Updater::Update(): this->activeQueue->Size() = " << this->activeQueue->Size() << std::endl)
	
	while(this->activeQueue->Size() != 0){
		if(this->activeQueue->Front().first > curTime){
			break;
		}
		this->UpdateUpdateable(this->activeQueue->PopFront());
	}
	
	this->AddPending();
	
	//After updating all the stuff some time has passed, so might need to correct the time need to wait
	
	ting::u32 closestTime;
	if(this->activeQueue->Size() != 0){
		ASSERT(curTime <= this->activeQueue->Front().first)
		closestTime = this->activeQueue->Front().first;
	}else if(this->inactiveQueue->Size() != 0){
		ASSERT(curTime > this->inactiveQueue->Front().first)
		closestTime = this->inactiveQueue->Front().first;
	}else{
		return ting::u32(-1);
	}
	
	ting::u32 uncorrectedDt = closestTime - curTime;
	
	ting::u32 correction = ting::timer::GetTicks() - curTime;
	
	if(correction >= uncorrectedDt){
		return 0;
	}else{
		return uncorrectedDt - correction;
	}
}



void Updateable::StartUpdating(ting::u16 dt){
	ASSERT(App::Inst().ThisIsUIThread())

	TRACE(<< "Updateable::StartUpdating(): this->IsUpdating() = " << this->IsUpdating() << std::endl)
	
	if(this->IsUpdating()){
		//TODO:
	}
	
	this->dt = dt;
	this->isUpdating = true;
	this->startedAt = ting::timer::GetTicks();
	
	App::Inst().updater.toAdd.push_front(ting::Ref<morda::Updateable>(this));
}
