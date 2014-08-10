/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <ting/Shared.hpp>

#include <cstdint>
#include <list>

#include "Exc.hpp"



namespace morda{



class Updateable : virtual public ting::Shared{
	friend class App;
	
private:
	class Updater{
		friend class morda::Updateable;
		
		typedef std::pair<std::uint32_t, std::weak_ptr<morda::Updateable>> T_Pair;
		
		class UpdateQueue : public std::list<T_Pair>{
		public:
			UpdateQueue::iterator Insert(const T_Pair& p);

			std::shared_ptr<morda::Updateable> PopFront(){
				std::shared_ptr<morda::Updateable> ret = std::move(this->front().second.lock());
				this->pop_front();
				return std::move(ret);
			}
		};
		
		UpdateQueue q1, q2;
		
		UpdateQueue *activeQueue, *inactiveQueue;
		
		std::uint32_t lastUpdatedTimestamp = 0;
		
		typedef std::list<std::shared_ptr<morda::Updateable> > T_ToAddList;
		T_ToAddList toAdd;
		
		void AddPending();
		
		void UpdateUpdateable(const std::shared_ptr<morda::Updateable>& u);
	public:
		Updater() :
				activeQueue(&q1),
				inactiveQueue(&q2)
		{}
		
		void RemoveFromToAdd(Updateable* u);
		
		//returns dt to wait before next update
		std::uint32_t Update();
	};
	
private:
	std::uint16_t dt;
	
	std::uint32_t startedAt; //timestamp when update timer started.
	
	std::uint32_t EndAt()const NOEXCEPT{
		return this->startedAt + std::uint32_t(this->dt);
	}
	
	bool isUpdating = false;
	
	//pointer to the queue the updateable is inserted into
	Updater::UpdateQueue* queue = nullptr;
	
	Updater::UpdateQueue::iterator iter; //iterator into the queue.
	
	bool pendingAddition = false;
	
public:
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};
	
	~Updateable()NOEXCEPT{}
	
	bool IsUpdating()const NOEXCEPT{
		return this->isUpdating;
	}
	
	void StartUpdating(std::uint16_t dt = 30);
	
	void StopUpdating()NOEXCEPT;
	
	virtual void Update(std::uint32_t dt) = 0;
};

}//~namespace
