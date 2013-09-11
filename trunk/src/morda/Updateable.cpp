#include "Updateable.hpp"

#include "App.hpp"

#include <ting/timer.hpp>



using namespace morda;



Updateable::Updater::UpdateQueue::iterator Updateable::Updater::UpdateQueue::Insert(const T_Pair& p){
	if(this->size() == 0 || this->back().first <= p.first){
		this->push_back(p);
		return --(this->end());
	}
	
	//otherwise, go from the beginning
	for(iterator i = this->begin(); i != this->end(); ++i){
		if(i->first >= p.first){
			return this->insert(i, p); //inserts before iterator
		}
	}
	
	ASSERT(false)
	return this->end();
}



void Updateable::Updater::AddPending(){
	while(this->toAdd.size() != 0){
		T_Pair p;
		
		p.first = this->toAdd.front()->EndAt();
		p.second = this->toAdd.front();
		
		if(p.first < this->lastUpdatedTimestamp){
//			TRACE(<< "Updateable::Updater::AddPending(): inserted to inactive queue" << std::endl)
			this->toAdd.front()->queue = this->inactiveQueue;
			this->toAdd.front()->iter = this->inactiveQueue->Insert(p);
		}else{
//			TRACE(<< "Updateable::Updater::AddPending(): inserted to active queue" << std::endl)
			this->toAdd.front()->queue = this->activeQueue;
			this->toAdd.front()->iter = this->activeQueue->Insert(p);
		}
		
		this->toAdd.front()->pendingAddition = false;
		
		this->toAdd.pop_front();
	}
}



void Updateable::Updater::UpdateUpdateable(const ting::Ref<morda::Updateable>& u){
	//if weak ref gave invalid strong ref
	if(!u){
		return;
	}
	
	//at this point updateable is removed from update queue, so set it to 0
	u->queue = 0;
	
	u->Update(this->lastUpdatedTimestamp - u->startedAt);
	
	//if not stopped during update, add it back
	if(u->IsUpdating()){
		u->startedAt = this->lastUpdatedTimestamp;
		u->pendingAddition = true;
		this->toAdd.push_back(u);
	}
}



ting::u32 Updateable::Updater::Update(){
	ting::u32 curTime = ting::timer::GetTicks();
	
//	TRACE(<< "Updateable::Updater::Update(): invoked" << std::endl)
	
	this->AddPending();//add pending before updating this->lastUpdatedTimestamp
	
	//check if there is a warp around
	if(curTime < this->lastUpdatedTimestamp){
		this->lastUpdatedTimestamp = curTime;
		
//		TRACE(<< "Updateable::Updater::Update(): time has warped, this->activeQueue->Size() = " << this->activeQueue->size() << std::endl)
		
		//if time has warped, then all Updateables from active queue have expired.
		while(this->activeQueue->size() != 0){
			this->UpdateUpdateable(this->activeQueue->PopFront());
		}
		
		std::swap(this->activeQueue, this->inactiveQueue);
	}else{
		this->lastUpdatedTimestamp = curTime;
	}
	ASSERT(this->lastUpdatedTimestamp == curTime)
	
//	TRACE(<< "Updateable::Updater::Update(): this->activeQueue->Size() = " << this->activeQueue->size() << std::endl)
	
	while(this->activeQueue->size() != 0){
		if(this->activeQueue->front().first > curTime){
			break;
		}
		this->UpdateUpdateable(this->activeQueue->PopFront());
	}
	
	this->AddPending();//after updating need to add recurring Updateables if any
	
	//After updating all the stuff some time has passed, so might need to correct the time need to wait
	
	ting::u32 closestTime;
	if(this->activeQueue->size() != 0){
		ASSERT(curTime <= this->activeQueue->front().first)
		closestTime = this->activeQueue->front().first;
	}else if(this->inactiveQueue->size() != 0){
		ASSERT(curTime > this->inactiveQueue->front().first)
		closestTime = this->inactiveQueue->front().first;
	}else{
		return ting::u32(-1);
	}
	
	ting::u32 uncorrectedDt = closestTime - curTime;
	
	ting::u32 correction = ting::timer::GetTicks() - curTime;
	
	if(correction >= uncorrectedDt){
		return 0;
	}else{
		uncorrectedDt -= correction;
		
		//do the updating with 10ms steps at least
		uncorrectedDt += (uncorrectedDt % 10 == 0 ? 0 : 10);
		
		return uncorrectedDt;
	}
}



void Updateable::Updater::RemoveFromToAdd(Updateable* u){
	ASSERT(u->pendingAddition)
	for(T_ToAddList::iterator i = this->toAdd.begin(); i != this->toAdd.end(); ++i){
		if((*i).operator->() == u){
			ASSERT((*i)->pendingAddition)
			u->pendingAddition = false;
			this->toAdd.erase(i);
			return;
		}
	}
}



void Updateable::StartUpdating(ting::u16 dt){
	ASSERT(App::Inst().ThisIsUIThread())

//	TRACE(<< "Updateable::StartUpdating(): this->IsUpdating() = " << this->IsUpdating() << std::endl)
	
	if(this->IsUpdating()){
		throw Exc("Updateable::StartUpdating(): Already updating");
	}
	
	this->dt = dt;
	this->startedAt = ting::timer::GetTicks();
	this->isUpdating = true;
	
	this->pendingAddition = true;
	
	App::Inst().updater.toAdd.push_front(ting::Ref<morda::Updateable>(this));
}



void Updateable::StopUpdating()throw(){
	ASSERT(App::Inst().ThisIsUIThread())
	
	if(this->queue){
		this->queue->erase(this->iter);
		this->queue = 0;
	}else if(this->pendingAddition){
		App::Inst().updater.RemoveFromToAdd(this);
	}

	this->isUpdating = false;
}
