/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "updater.hpp"

#include <chrono>

#include <utki/time.hpp>

#include "updateable.hpp"

using namespace morda;

void updater::start(std::weak_ptr<updateable> u, uint16_t dt_ms){
    auto uu = u.lock();
    if(!uu){
        return;
    }

    if(uu->is_updating()){
		throw std::logic_error("updater::start(): updateable is already being updated");
	}
	
	uu->dt = dt_ms;
	uu->startedAt = utki::get_ticks_ms();
	uu->is_updating_v = true;
	
	uu->pendingAddition = true;
	
	this->to_add.push_front(uu); //TODO: add weak ptr
}

void updater::stop(updateable& u)noexcept{
    if(u.queue){
		u.queue->erase(u.iter);
		u.queue = 0;
	}else if(u.pendingAddition){
		this->removeFromToAdd(&u);
	}

	u.is_updating_v = false;
}

void updater::removeFromToAdd(updateable* u){
	ASSERT(u->pendingAddition)
	for(auto i = this->to_add.begin(); i != this->to_add.end(); ++i){
		if((*i).operator->() == u){
			ASSERT((*i)->pendingAddition)
			u->pendingAddition = false;
			this->to_add.erase(i);
			return;
		}
	}
}


updater::update_queue::iterator updater::update_queue::insert(const T_Pair& p){
	if(this->size() == 0 || this->back().first <= p.first){
		this->push_back(p);
		return --(this->end());
	}
	
	// otherwise, go from the beginning
	for(auto i = this->begin(); i != this->end(); ++i){
		if(i->first >= p.first){
			return this->list::insert(i, p); // inserts before iterator
		}
	}
	
	ASSERT(false)
	return this->end();
}



void updater::addPending(){
	while(this->to_add.size() != 0){
		T_Pair p;
		
		p.first = this->to_add.front()->ends_at();
		p.second = this->to_add.front();
		
		if(p.first < this->lastUpdatedTimestamp){
//			TRACE(<< "updateable::Updater::AddPending(): inserted to inactive queue" << std::endl)
			this->to_add.front()->queue = this->inactiveQueue;
			this->to_add.front()->iter = this->inactiveQueue->insert(p);
		}else{
//			TRACE(<< "updateable::Updater::AddPending(): inserted to active queue" << std::endl)
			this->to_add.front()->queue = this->activeQueue;
			this->to_add.front()->iter = this->activeQueue->insert(p);
		}
		
		this->to_add.front()->pendingAddition = false;
		
		this->to_add.pop_front();
	}
}



void updater::updateUpdateable(const std::shared_ptr<morda::updateable>& u){
	//if weak ref gave invalid strong ref
	if(!u){
		return;
	}
	
	//at this point updateable is removed from update queue, so set it to 0
	u->queue = 0;
	
	u->update(this->lastUpdatedTimestamp - u->startedAt);
	
	//if not stopped during update, add it back
	if(u->is_updating()){
		u->startedAt = this->lastUpdatedTimestamp;
		u->pendingAddition = true;
		this->to_add.push_back(u);
	}
}



uint32_t updater::update(){
	uint32_t curTime = utki::get_ticks_ms();
	
//	TRACE(<< "updateable::Updater::Update(): invoked" << std::endl)
	
	this->addPending(); // add pending before updating this->lastUpdatedTimestamp
	
	// check if there is a warp around
	if(curTime < this->lastUpdatedTimestamp){
		this->lastUpdatedTimestamp = curTime;
		
//		TRACE(<< "updateable::Updater::Update(): time has warped, this->activeQueue->Size() = " << this->activeQueue->size() << std::endl)
		
		//if time has warped, then all Updateables from active queue have expired.
		while(this->activeQueue->size() != 0){
			this->updateUpdateable(this->activeQueue->pop_front());
		}
		
		std::swap(this->activeQueue, this->inactiveQueue);
	}else{
		this->lastUpdatedTimestamp = curTime;
	}
	ASSERT(this->lastUpdatedTimestamp == curTime)
	
//	TRACE(<< "updateable::Updater::Update(): this->activeQueue->Size() = " << this->activeQueue->size() << std::endl)
	
	while(this->activeQueue->size() != 0){
		if(this->activeQueue->front().first > curTime){
			break;
		}
		this->updateUpdateable(this->activeQueue->pop_front());
	}
	
	this->addPending(); // after updating need to add recurring Updateables if any
	
	//After updating all the stuff some time has passed, so might need to correct the time need to wait
	
	uint32_t closestTime;
	if(this->activeQueue->size() != 0){
		ASSERT(curTime <= this->activeQueue->front().first)
		closestTime = this->activeQueue->front().first;
	}else if(this->inactiveQueue->size() != 0){
		ASSERT(curTime > this->inactiveQueue->front().first)
		closestTime = this->inactiveQueue->front().first;
	}else{
		return uint32_t(-1);
	}
	
	uint32_t uncorrectedDt = closestTime - curTime;
	
	uint32_t correction = utki::get_ticks_ms() - curTime;
	
	if(correction >= uncorrectedDt){
		return 0;
	}else{
		uncorrectedDt -= correction;
		
		if(0 < uncorrectedDt && uncorrectedDt < 5){
			uncorrectedDt = 5; // wait for 5 ms at least, if not 0.
		}
		
		return uncorrectedDt;
	}
}
