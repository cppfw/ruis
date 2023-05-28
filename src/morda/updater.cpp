/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include <utki/time.hpp>

#include "updateable.hpp"

using namespace morda;

void updater::start(std::weak_ptr<updateable> u, uint16_t dt_ms)
{
	auto uu = u.lock();
	if (!uu) {
		return;
	}

	if (uu->is_updating()) {
		throw std::logic_error("updater::start(): updateable is already being updated");
	}

	uu->dt = dt_ms;
	uu->started_at = utki::get_ticks_ms();
	uu->updating = true;

	uu->pending_addition = true;

	this->to_add.push_front(uu); // TODO: add weak ptr
}

void updater::stop(updateable& u) noexcept
{
	if (u.queue) {
		u.queue->erase(u.iter);
		u.queue = nullptr;
	} else if (u.pending_addition) {
		this->remove_from_to_add(&u);
	}

	u.updating = false;
}

void updater::remove_from_to_add(updateable* u)
{
	ASSERT(u->pending_addition)
	for (auto i = this->to_add.begin(); i != this->to_add.end(); ++i) {
		if ((*i).operator->() == u) {
			ASSERT((*i)->pending_addition)
			u->pending_addition = false;
			this->to_add.erase(i);
			return;
		}
	}
}

updater::update_queue::iterator updater::update_queue::insert(const update_queue_item& p)
{
	if (this->size() == 0 || this->back().ends_at <= p.ends_at) {
		this->push_back(p);
		return std::prev(this->end());
	}

	// otherwise, go from the beginning
	for (auto i = this->begin(); i != this->end(); ++i) {
		if (i->ends_at >= p.ends_at) {
			return this->list::insert(i, p); // inserts before iterator
		}
	}

	ASSERT(false)
	return this->end();
}

void updater::add_pending()
{
	while (this->to_add.size() != 0) {
		update_queue_item p;

		p.ends_at = this->to_add.front()->ends_at();
		p.updateable = this->to_add.front();

		if (p.ends_at < this->last_updated_timestamp) {
			//			TRACE(<< "updateable::Updater::AddPending(): inserted to inactive queue" << std::endl)
			this->to_add.front()->queue = this->inactive_queue;
			this->to_add.front()->iter = this->inactive_queue->insert(p);
		} else {
			//			TRACE(<< "updateable::Updater::AddPending(): inserted to active queue" << std::endl)
			this->to_add.front()->queue = this->active_queue;
			this->to_add.front()->iter = this->active_queue->insert(p);
		}

		this->to_add.front()->pending_addition = false;

		this->to_add.pop_front();
	}
}

void updater::update_updateable(const std::shared_ptr<morda::updateable>& u)
{
	// if weak ref gave invalid strong ref
	if (!u) {
		return;
	}

	// at this point updateable is removed from update queue, so set it to nullptr
	u->queue = nullptr;

	u->update(this->last_updated_timestamp - u->started_at);

	// if not stopped during update, add it back
	if (u->is_updating()) {
		u->started_at = this->last_updated_timestamp;
		u->pending_addition = true;
		this->to_add.push_back(u);
	}
}

uint32_t updater::update()
{
	uint32_t cur_ticks = utki::get_ticks_ms();

	//	TRACE(<< "updateable::Updater::Update(): invoked" << std::endl)

	this->add_pending(); // add pending before updating this->last_updated_timestamp

	// check if there is a warp around
	if (cur_ticks < this->last_updated_timestamp) {
		this->last_updated_timestamp = cur_ticks;

		//		TRACE(<< "updateable::Updater::Update(): time has warped, this->active_queue->Size() = " <<
		// this->active_queue->size() << std::endl)

		// if time has warped, then all Updateables from active queue have expired.
		while (this->active_queue->size() != 0) {
			this->update_updateable(this->active_queue->pop_front());
		}

		std::swap(this->active_queue, this->inactive_queue);
	} else {
		this->last_updated_timestamp = cur_ticks;
	}
	ASSERT(this->last_updated_timestamp == cur_ticks)

	//	TRACE(<< "updateable::Updater::Update(): this->active_queue->Size() = " << this->active_queue->size() <<
	// std::endl)

	while (this->active_queue->size() != 0) {
		if (this->active_queue->front().ends_at > cur_ticks) {
			break;
		}
		this->update_updateable(this->active_queue->pop_front());
	}

	this->add_pending(); // after updating need to add recurring Updateables if any

	// after updating all the stuff some time has passed, so might need to correct the time need to wait

	uint32_t closest_time_point;
	if (this->active_queue->size() != 0) {
		ASSERT(cur_ticks <= this->active_queue->front().ends_at)
		closest_time_point = this->active_queue->front().ends_at;
	} else if (this->inactive_queue->size() != 0) {
		ASSERT(cur_ticks > this->inactive_queue->front().ends_at)
		closest_time_point = this->inactive_queue->front().ends_at;
	} else {
		return uint32_t(-1);
	}

	uint32_t uncorrected_dt = closest_time_point - cur_ticks;

	uint32_t correction = utki::get_ticks_ms() - cur_ticks;

	if (correction >= uncorrected_dt) {
		return 0;
	} else {
		uncorrected_dt -= correction;

		if (0 < uncorrected_dt && uncorrected_dt < 5) {
			uncorrected_dt = 5; // wait for 5 ms at least, if not 0.
		}

		return uncorrected_dt;
	}
}
