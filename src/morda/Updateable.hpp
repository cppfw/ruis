/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <utki/Shared.hpp>

#include <cstdint>
#include <list>

#include "Exc.hpp"



namespace morda{



class Updateable : virtual public utki::Shared{
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
	
	std::uint32_t EndAt()const noexcept{
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
	
	~Updateable()noexcept{}
	
	bool IsUpdating()const noexcept{
		return this->isUpdating;
	}
	
	void StartUpdating(std::uint16_t dt = 30);
	
	void StopUpdating()noexcept;
	
	virtual void Update(std::uint32_t dt) = 0;
};

}//~namespace
