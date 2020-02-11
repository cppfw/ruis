#include "updater.hpp"

#include <chrono>

#include <utki/exception.hpp>

#include "updateable.hpp"

using namespace morda;

namespace{
std::uint32_t get_ticks(){
	return std::uint32_t(
			std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
				).count()
		);
}
}

void updater::start(std::weak_ptr<updateable> u, uint16_t dt_ms){
    auto uu = u.lock();
    if(!uu){
        return;
    }

    if(uu->isUpdating()){
		throw utki::invalid_state("updater::start(): updateable is already being updated");
	}
	
	uu->dt = dt_ms;
	uu->startedAt = get_ticks();
	uu->isUpdating_v = true;
	
	uu->pendingAddition = true;
	
	this->toAdd.push_front(uu); //TODO: add weak ptr
}

void updater::stop(updateable& u)noexcept{
    if(u.queue){
		u.queue->erase(u.iter);
		u.queue = 0;
	}else if(u.pendingAddition){
		this->removeFromToAdd(&u);
	}

	u.isUpdating_v = false;
}
