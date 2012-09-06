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


#include <ting/Ref.hpp>



namespace morda{



class Updateable : virtual public ting::RefCounted{
	friend class App;
	
	ting::u16 dt;
	
	ting::u32 startedAt; //timestamp when update timer started.
	
	ting::Inited<bool, false> isUpdating;
	
public:
	~Updateable()throw(){}
	
	bool IsUpdating()const throw(){
		return this->isUpdating;
	}
	
	void StartUpdating(ting::u16 dt = 30);
	
	void StopUpdating();
	
	virtual void Update(ting::u32 dt) = 0;
	
private:
	class Updater{
		class UpdateQueue{
		public:
			typedef std::pair<ting::u32, ting::WeakRef<morda::Updateable> > T_Pair;
		private:
			std::list<T_Pair> l;
		public:
			inline size_t Size()const throw(){
				return this->l.size();
			}

			void Insert();

			inline const T_Pair& Front()const throw(){
				return this->l.front();
			}

			inline ting::Ref<morda::Updateable> PopFront(){
				ting::Ref<morda::Updateable> ret = this->l.front().second;
				this->l.pop_front();
				return ret;
			}
		};
		
		UpdateQueue q1, q2;
		
		UpdateQueue *activeQueue, *inactiveQueue;
		
		ting::Inited<bool, true> lastUpdateWasInFirstHalf;
		
		std::list<ting::Ref<morda::Updateable> > toAdd;
		
		void AddPending();
	public:
		Updater() :
				activeQueue(&q1),
				inactiveQueue(&q2)
		{}
		
		//returns dt to wait before next update
		ting::u32 Update();
	};
};

}//~namespace
