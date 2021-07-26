/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#pragma once

#include <memory>

#include <list>

namespace morda{

class updateable;

class updater : public std::enable_shared_from_this<updater>{
	friend class morda::updateable;

	typedef std::pair<uint32_t, std::weak_ptr<morda::updateable>> T_Pair;

	class UpdateQueue : public std::list<T_Pair>{
	public:
		UpdateQueue::iterator insertPair(const T_Pair& p);

		std::shared_ptr<morda::updateable> popFront(){
			auto ret = this->front().second.lock();
			this->pop_front();
			return ret;
		}
	};

	UpdateQueue q1, q2;

	UpdateQueue *activeQueue, *inactiveQueue;

	uint32_t lastUpdatedTimestamp = 0;

	typedef std::list<std::shared_ptr<morda::updateable> > T_ToAddList;
	T_ToAddList toAdd;

	void addPending();

	void updateUpdateable(const std::shared_ptr<updateable>& u);

	void removeFromToAdd(updateable* u);
public:
	updater() :
			activeQueue(&q1),
			inactiveQueue(&q2)
	{}

	// returns dt to wait before next update
	uint32_t update();

	/**
	 * @brief Subscribe updateable for updates.
	 * Normally, updates will start from the next UI cycle.
	 * The virtual update() method will be called periodically.
	 * Due to limitations specific to each particular platform the actual period
	 * can be different from requested period.
	 * The function is not thread safe, must be called from UI thread.
	 * @param u - updateable to start updating.
	 * @param dt_ms - time period between updates, in milliseconds.
	 */
	void start(std::weak_ptr<updateable> u, uint16_t dt_ms = 30);

	/**
	 * @brief Unsubscribe from updating.
	 * After calling this method there will be no updates done.
	 * The function is not thread safe, must be called fom UI thread.
	 * @param u - updateble to stop updating.
	 */
	void stop(updateable& u)noexcept;
};

}
