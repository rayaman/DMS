#pragma once
#include "connection.h"
#include <unordered_map>
#include <chrono>
#include <thread>
#include <iostream>
namespace multi {
	// Keeps track of time.
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::nanoseconds nanoseconds;
	typedef std::chrono::microseconds microseconds;
	typedef std::chrono::milliseconds milliseconds;
	typedef std::chrono::seconds seconds;
	typedef std::chrono::minutes minutes;
	typedef std::chrono::hours hours;

	const char _nanosecond = 0x0;
	const char _microsecond = 0x1;
	const char _millisecond = 0x2;
	const char _second = 0x3;
	const char _minute = 0x4;
	const char _hour = 0x5;

	typedef std::chrono::time_point<std::chrono::high_resolution_clock> time;
	template <typename T>
	struct timer {
		inline void start() {
			t = Clock::now();
		}
		inline long long get() const {
			return std::chrono::duration_cast<T>(Clock::now() - t).count();
		}
		inline void sleep(long long s) {
			std::this_thread::sleep_for(T(s));
		}
	private:
		time t;
	};
	class mbase;
	enum class mtype { mEvent, mStep, mLoop, mAlarm, mTStep, mTLoop, mService, mUpdater };
	const std::unordered_map<mtype, const char*> mtypes { 
		{mtype::mEvent,"Event"},
		{mtype::mStep,"Step"},
		{mtype::mLoop,"Loop"},
		{mtype::mAlarm,"Alarm"},
		{mtype::mTStep,"TStep"},
		{mtype::mTLoop,"TLoop"},
		{mtype::mService,"Service"},
		{mtype::mUpdater,"Updater"}
	};
	/// <summary>
	/// Class responsible for driving multi objects
	/// </summary>
	class runner {
		friend class mbase;
		bool active = true;
		std::vector<mbase*> mObjects;
	public:
		bool update();
		void mainloop();
		/// <summary>
		/// Event Triggered when the multi object is created
		/// </summary>
		connection<mbase*> OnCreate;
		/// <summary>
		/// Event Triggered when the multi object is destroyed
		/// </summary>
		connection<mbase*> OnDestroyed;
	};
	class mbase {
	public:
		runner* Parent = nullptr;
		const mtype Type;
		const bool active = true;
		const bool paused = false;
		mbase(mtype type, runner* run) : Type(type) {
			Parent = run;
			run->mObjects.push_back(this);
		}
		virtual void act(int mid) {}
		virtual void reset() { const_cast<bool&>(active) = true; };
		void pause() {
			if (!paused)
				OnPaused.fire(this);
			const_cast<bool&>(paused) = true;
		}
		void resume() {
			if (paused)
				OnResume.fire(this);
			const_cast<bool&>(paused) = false;
		}
		void stop() {
			OnStopped.fire(this);
			const_cast<bool&>(active) = false;
		}
		/// <summary>
		/// Event Triggered when the multi object is paused
		/// </summary>
		connection<mbase*> OnPaused;
		/// <summary>
		/// Event Triggered when the multi object is resumed
		/// </summary>
		connection<mbase*> OnResume;
		/// <summary>
		/// Event Triggered when the multi object is stopped
		/// </summary>
		connection<mbase*> OnStopped;
	};
}