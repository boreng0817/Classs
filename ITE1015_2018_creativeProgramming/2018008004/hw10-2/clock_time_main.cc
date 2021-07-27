#include "clock_time.h"
#include <iostream>
#include <vector>
using namespace std;

class Clock{
	    public:
			        Clock(int hour, int minute, int second, double driftPerSecond);
			        void reset();
			        void tick();
			        virtual void displayTime() = 0;
	    protected:
			        ClockTime _clockTime;
			        double _driftPerSecond;
			        double _totalDrift;
};
class NaturalClock : public Clock{
	    public:
			        NaturalClock(int hour, int minute, int second, double driftPerSecond) : Clock(hour,minute,second,driftPerSecond) {}

};
class MechanicalClock : public Clock{
	    public:
			        MechanicalClock(int hour, int minute, int second, double driftPerSecond) : Clock(hour,minute,second,driftPerSecond) {}
};
class DigitalClock : public Clock{
	    public:
			        DigitalClock(int hour, int minute, int second, double driftPerSecond) : Clock(hour,minute,second,driftPerSecond) {}
};
class QuantumClock : public Clock{
	    public:
			        QuantumClock(int hour, int minute, int second, double driftPerSecond) : Clock(hour,minute,second,driftPerSecond) {}
};
class SundialClock : public NaturalClock{
	    public:
			        SundialClock(int hour, int minute, int second) : NaturalClock(hour,minute,second,0.0) {}
					        void displayTime();
};
class CuckooClock : public MechanicalClock{
	    public:
			        CuckooClock(int hour, int minute, int second) : MechanicalClock(hour,minute,second,0.0) {}
					        void displayTime();
};
class GrandFatherClock : public MechanicalClock{
    public:
        GrandFatherClock(int hour, int minute, int second) : MechanicalClock(hour,minute,second,0.000694444) {}
        void displayTime();
};
class WristClock : public DigitalClock{
    public:
        WristClock(int hour, int minute, int second) : DigitalClock(hour,minute,second,0.000347222) {}
        void displayTime();
};
class AtomicClock : public QuantumClock{
    public:
        AtomicClock(int hour, int minute, int second) : QuantumClock(hour,minute,second,0.000034722) {}
        void displayTime();
};

Clock::
Clock(int hour, int minute, int second, double driftPerSecond){
    _clockTime.setTime(hour,minute,second,1);
    _driftPerSecond = driftPerSecond;
    _totalDrift = 0;
}

void Clock::
reset()
{
    _clockTime.reset();
}

void Clock::
tick()
{
    _clockTime.increment();
    _totalDrift += _driftPerSecond;
}

void SundialClock::
displayTime()
{
    cout << "SundialClock ";
    _clockTime.display();
    cout << ", total drift: " << _totalDrift;
    cout << endl;
}

void CuckooClock::
displayTime()
{
    cout << "CuckooClock ";
    _clockTime.display();
    cout << ", total drift: " << _totalDrift;
    cout << endl;
}

void GrandFatherClock::
displayTime()
{
    cout << "GrandFatherClock ";
    _clockTime.display();
    cout << ", total drift: " << _totalDrift;
    cout << endl;
}

void WristClock::
displayTime()
{
    cout << "WristClock ";
    _clockTime.display();
    cout << ", total drift: " << _totalDrift;
    cout << endl;
}

void AtomicClock::
displayTime()
{
    cout << "AtomicClock ";
    _clockTime.display();
    cout << ", total drift: " << _totalDrift;
    cout << endl;
}



int main(){
	vector<Clock*> v;
	int tick;
	cin >> tick;
	v.push_back(new SundialClock(0,0,0));
	v.push_back(new CuckooClock(0,0,0));
	v.push_back(new GrandFatherClock(0,0,0));
	v.push_back(new WristClock(0,0,0));
	v.push_back(new AtomicClock(0,0,0));

	for(int i = 0 ; i < v.size() ; i++) v[i]->reset();
	cout << "Reported clock times after resetting:"<<endl;
	for(int i = 0 ; i < v.size() ; i++) v[i]->displayTime();
	
	cout << endl;
	cout << "Running the clocks..."<<endl;
	cout << endl;

	for(int i = 0 ; i < tick ; i++)
		for(int k = 0 ; k < v.size() ; k++) v[k]->tick();
	cout << "Reported clock times after running:"<<endl;
	for(int i = 0 ; i < v.size() ; i++) {v[i]->displayTime();delete v[i];}
	
	v.clear();
	return 0;
}


