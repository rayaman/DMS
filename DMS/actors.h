#pragma once
#include "multibase.h"
#include <chrono>
namespace multi {
	class event : public mbase {
	public:
		event(runner* run, void(*func)(event*)) : mbase(mtype::mEvent,run) {
			OnEvent += func;
		}
		event(runner* run) : mbase(mtype::mEvent,run) {}
		connection<event*> OnEvent;
	};
	template <typename T>
	class alarm : public mbase {
		long long milli;
		timer<T> t;
	public:
		alarm(runner* run, long long milli, std::function<void(alarm*)> func) : mbase(mtype::mAlarm,run) {
			this->milli = milli;
			OnRing += func;
			t.start();
		}
		alarm(runner* run, long long milli) : mbase(mtype::mAlarm,run) {
			this->milli = milli;
			t.start();
		}
		alarm(runner* run, void(*func)(alarm*)) : mbase(mtype::mAlarm,run) {
			OnRing += func;
			milli = 1000;
			t.start();
		}
		alarm(runner* run) : mbase(mtype::mAlarm, run) { 
			milli = 1000; 
			t.start(); 
		};
		void set(long long milli) {
			this->milli = milli;
			t.start();
		}
		void reset(long long milli) {
			set(milli);
			mbase::reset();
		}
		void reset() {
			t.start();
			mbase::reset();
		}
		void act(int id) override {
			if (!active) return;
			if (t.get() >= milli) {
				stop();
				OnRing.fire(this);
			}
		}
		connection<alarm*> OnRing;
	};
	class step : public mbase {
		int start;
		int endAt;
		int count;
		bool loop;
		int pos;
	public:
		step(runner* run, int start, int endAt, int count = 1, bool loop = false) : mbase(mtype::mStep,run) {
			update(start, endAt, count, loop);
		}
		void reset() {
			pos = start;
			mbase::reset();
		}
		void update(int start, int endAt, int count = 1, bool loop = false) {
			this->start = start;
			pos = start;
			this->endAt = endAt;
			this->count = count;
			if (start > endAt && count == 1)
				this->count = -1;
			this->loop = loop;
		}
		void act(int id) override {
			if (!active) return;
			if (pos == start) OnStepBegin.fire(this);
			OnStep.fire(pos);
			pos += count;
			if ((count > start && pos > endAt) || (count < start && pos < endAt)) {
				stop();
				OnStepEnd.fire(this);
			}
		}
		connection<step*> OnStepBegin; // before all steps start
		connection<int> OnStep; // each step
		connection<step*> OnStepEnd; // After end of all steps
	};
	class loop : public mbase {
	public:
		loop(runner* run,void(*func)(loop*)) : mbase(mtype::mLoop,run) {
			OnLoop += func;
		}
		void act(int id) override {
			if (!active) return;
			OnLoop.fire(this);
		}
		loop(runner* run) : mbase(mtype::mLoop,run) {}
		connection<loop*> OnLoop;
	};
	template<typename T>
	class tstep : public mbase {
		long long milli;
		timer<T> t;
		int start;
		int endAt;
		int count;
		bool loop;
		int pos;
	public:
		tstep(runner* run, long long milli, int start, int endAt, int count = 1, bool loop = false) : mbase(mtype::mTStep, run) {
			update(start, endAt, count, loop);
			reset(milli);
		}
		void reset() override {
			pos = start;
			mbase::reset();
		}
		void reset(long long milli) {
			pos = start;
			set(milli);
			mbase::reset();
		}
		void set(long long milli) {
			this->milli = milli;
			t.start();
		}
		void update(int start, int endAt, int count = 1, bool loop = false) {
			this->start = start;
			pos = start;
			this->endAt = endAt;
			this->count = count;
			if (start > endAt && count == 1)
				this->count = -1;
			this->loop = loop;
		}
		void act(int id) override {
			if (!active) return;
			if (t.get() >= milli) {
				if (pos == start) OnStepBegin.fire(this);
				OnStep.fire(pos);
				pos += count;
				t.start();
				if ((count > start && pos > endAt) || (count < start && pos < endAt)) {
					stop();
					OnStepEnd.fire(this);
				}
			}
		}
		connection<tstep*> OnStepBegin; // before all steps start
		connection<int> OnStep; // each step
		connection<tstep*> OnStepEnd; // After end of all steps
	};
	template <typename T>
	class tloop : public mbase {
		long long milli;
		timer<T> t;
	public:
		tloop(runner* run, long long milli, void(*func)(tloop*)) : mbase(mtype::mTLoop, run) {
			OnLoop += func;
			this->milli = milli;
		}
		void set(long long milli) {
			this->milli = milli;
			t.start();
		}
		void act(int id) override {
			if (!active) return;
			if (t.get() >= milli) {
				OnLoop.fire(this);
				t.start();
				reset();
			}
		}
		connection<tloop*> OnLoop;
	};
}