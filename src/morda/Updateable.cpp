#include "Updateable.hpp"

#include "context.hpp"

#include <chrono>


using namespace morda;



namespace{
std::uint32_t getTicks(){
	return std::uint32_t(
			std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
				).count()
		);
}
}


Updater::UpdateQueue::iterator Updater::UpdateQueue::insertPair(const T_Pair& p){
	if(this->size() == 0 || this->back().first <= p.first){
		this->push_back(p);
		return --(this->end());
	}
	
	//otherwise, go from the beginning
	for(auto i = this->begin(); i != this->end(); ++i){
		if(i->first >= p.first){
			return this->insert(i, p); //inserts before iterator
		}
	}
	
	ASSERT(false)
	return this->end();
}



void Updater::addPending(){
	while(this->toAdd.size() != 0){
		T_Pair p;
		
		p.first = this->toAdd.front()->endAt();
		p.second = this->toAdd.front();
		
		if(p.first < this->lastUpdatedTimestamp){
//			TRACE(<< "Updateable::Updater::AddPending(): inserted to inactive queue" << std::endl)
			this->toAdd.front()->queue = this->inactiveQueue;
			this->toAdd.front()->iter = this->inactiveQueue->insertPair(p);
		}else{
//			TRACE(<< "Updateable::Updater::AddPending(): inserted to active queue" << std::endl)
			this->toAdd.front()->queue = this->activeQueue;
			this->toAdd.front()->iter = this->activeQueue->insertPair(p);
		}
		
		this->toAdd.front()->pendingAddition = false;
		
		this->toAdd.pop_front();
	}
}



void Updater::updateUpdateable(const std::shared_ptr<morda::Updateable>& u){
	//if weak ref gave invalid strong ref
	if(!u){
		return;
	}
	
	//at this point updateable is removed from update queue, so set it to 0
	u->queue = 0;
	
	u->update(this->lastUpdatedTimestamp - u->startedAt);
	
	//if not stopped during update, add it back
	if(u->isUpdating()){
		u->startedAt = this->lastUpdatedTimestamp;
		u->pendingAddition = true;
		this->toAdd.push_back(u);
	}
}



std::uint32_t Updater::update(){
	std::uint32_t curTime = getTicks();
	
//	TRACE(<< "Updateable::Updater::Update(): invoked" << std::endl)
	
	this->addPending();//add pending before updating this->lastUpdatedTimestamp
	
	//check if there is a warp around
	if(curTime < this->lastUpdatedTimestamp){
		this->lastUpdatedTimestamp = curTime;
		
//		TRACE(<< "Updateable::Updater::Update(): time has warped, this->activeQueue->Size() = " << this->activeQueue->size() << std::endl)
		
		//if time has warped, then all Updateables from active queue have expired.
		while(this->activeQueue->size() != 0){
			this->updateUpdateable(this->activeQueue->popFront());
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
		this->updateUpdateable(this->activeQueue->popFront());
	}
	
	this->addPending();//after updating need to add recurring Updateables if any
	
	//After updating all the stuff some time has passed, so might need to correct the time need to wait
	
	std::uint32_t closestTime;
	if(this->activeQueue->size() != 0){
		ASSERT(curTime <= this->activeQueue->front().first)
		closestTime = this->activeQueue->front().first;
	}else if(this->inactiveQueue->size() != 0){
		ASSERT(curTime > this->inactiveQueue->front().first)
		closestTime = this->inactiveQueue->front().first;
	}else{
		return std::uint32_t(-1);
	}
	
	std::uint32_t uncorrectedDt = closestTime - curTime;
	
	std::uint32_t correction = getTicks() - curTime;
	
	if(correction >= uncorrectedDt){
		return 0;
	}else{
		uncorrectedDt -= correction;
		
		if(0 < uncorrectedDt && uncorrectedDt < 5){
			uncorrectedDt = 5;//wait for 5ms at least, if not 0.
		}
		
		return uncorrectedDt;
	}
}



void Updater::removeFromToAdd(Updateable* u){
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



void Updateable::startUpdating(std::uint16_t dtMs){
//	ASSERT(App::inst().thisIsUIThread())

//	TRACE(<< "Updateable::StartUpdating(): this->IsUpdating() = " << this->IsUpdating() << std::endl)
	
	if(this->isUpdating()){
		throw Exc("Updateable::StartUpdating(): Already updating");
	}
	
	this->dt = dtMs;
	this->startedAt = getTicks();
	this->isUpdating_v = true;
	
	this->pendingAddition = true;
	
	gui::inst().context->updater.toAdd.push_front(this->sharedFromThis(this));
}



void Updateable::stopUpdating()noexcept{
//	ASSERT(App::inst().thisIsUIThread())
	
	if(this->queue){
		this->queue->erase(this->iter);
		this->queue = 0;
	}else if(this->pendingAddition){
		gui::inst().context->updater.removeFromToAdd(this);
	}

	this->isUpdating_v = false;
}
