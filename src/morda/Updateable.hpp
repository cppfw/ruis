#pragma once

#include <cstdint>
#include <list>

#include <utki/shared.hpp>

#include "exception.hpp"



namespace morda{



/**
 * @brief Base class for periodic updating.
 * A subclass of this class can subscribe for periodic updates to be performed from UI thread.
 * For example every 30 milliseconds it will call the update() method.
 */
class Updateable : virtual public utki::shared{
	friend class Morda;

private:
	class Updater{
		friend class morda::Updateable;

		typedef std::pair<std::uint32_t, std::weak_ptr<morda::Updateable>> T_Pair;

		class UpdateQueue : public std::list<T_Pair>{
		public:
			UpdateQueue::iterator insertPair(const T_Pair& p);

			std::shared_ptr<morda::Updateable> popFront(){
				auto ret = this->front().second.lock();
				this->pop_front();
				return ret;
			}
		};

		UpdateQueue q1, q2;

		UpdateQueue *activeQueue, *inactiveQueue;

		std::uint32_t lastUpdatedTimestamp = 0;

		typedef std::list<std::shared_ptr<morda::Updateable> > T_ToAddList;
		T_ToAddList toAdd;

		void addPending();

		void updateUpdateable(const std::shared_ptr<morda::Updateable>& u);
	public:
		Updater() :
				activeQueue(&q1),
				inactiveQueue(&q2)
		{}

		void removeFromToAdd(Updateable* u);

		//returns dt to wait before next update
		std::uint32_t update();
	};

private:
	std::uint16_t dt;

	std::uint32_t startedAt; //timestamp when update timer started.

	std::uint32_t endAt()const noexcept{
		return this->startedAt + std::uint32_t(this->dt);
	}

	bool isUpdating_v = false;

	//pointer to the queue the updateable is inserted into
	Updater::UpdateQueue* queue = nullptr;

	Updater::UpdateQueue::iterator iter; //iterator into the queue.

	bool pendingAddition = false;

public:
	/**
	 * @brief Basic update-related Exception.
	 */
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};

	/**
	 * @brief Check if the object is currently subscribed for updates.
	 * @return true if object is subscribed for updates.
	 * @return false otherwise.
	 */
	bool isUpdating()const noexcept{
		return this->isUpdating_v;
	}

	/**
	 * @brief Subscribe for updates.
	 * Normally, updates will start from the next UI cycle.
	 * The virtual update() method will be called periodically.
	 * Due to limitations specific to each particular platform the actual period
	 * can be different from requested period.
	 * The function is not thread safe.
	 * @param dtMs - time period between updates, in milliseconds.
	 */
	void startUpdating(std::uint16_t dtMs = 30);

	/**
	 * @brief Unsubscribe from updating.
	 * After calling this method there will be no updates done.
	 * The function is not thread safe.
	 */
	void stopUpdating()noexcept;

	/**
	 * @brief A method to perform an update.
	 * Override this method to perform
	 * @param dtMs - actual time elapsed since the previous update.
	 */
	virtual void update(std::uint32_t dtMs) = 0;
};

}
