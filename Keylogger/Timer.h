/*We will need this header for:
-> Parallel Code Execution
-> Delayed  Code Execution
-> Repeated Code Execution ( Calling functions in certain intervals)
We will need this header for sending an email for every 12 hours
Multiple threads can share the same variables, the same data section, share the same memory address base
It can block the main thread or delay it, Chrono library for precise time calculation
Timer class will be used for insyncronous code execution
*/

#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer
{
    std::thread Thread;//Thread for asyncronous execution without blocking the main thread, it will a variable of type thread
    bool Alive = false;//Timer is running or not
    long CallNumber = -1L;//To determine how many times we would like to call a certain function
    long repeat_count = -1L;//Count of the amount time that a certain function has been called
    std::chrono::milliseconds interval = std::chrono::milliseconds(0); // Represent an interval between function calls
    std::function<void(void)> funct = nullptr ; //Return and input value and is like a pointer to a function

    void SleepAndRun()
    {
        std::this_thread::sleep_for(interval); /* This thread indicates the current thread which created the timer object
        and it will block the thread for a certain time of interval*/
        if(Alive)//if the timer is running call the function below
            Function()();//First parenthesis called the Function and the second one called the function that returned from the function
    }

    void ThreadFunc() // The function which passed to thread which tell to thread to do something 5 times etc
    {
        if(CallNumber == Infinite)
            while(Alive)
                SleepAndRun();
        else
            while(repeat_count--)
                SleepAndRun();
    }

    public:
        static const long Infinite = -1L; // Static without creating an instance

        Timer(){}//We will use this constructor to create a timer as a global variable accept the function we will call

        Timer(const std::function<void(void)> &f) : funct(f) {} // Other variables have default values

        Timer(const std::function<void(void)> &f,
              const unsigned long &i,
              const long repeat = Timer::Infinite) : funct(f) ,
              interval(std::chrono::milliseconds(i)),
              CallNumber(repeat) {}

        //Method for starting the timer
        void Start(bool Async = true)
        {
            if(IsAlive())
                return;
            Alive = true;
            repeat_count = CallNumber;
            if(Async)
                Thread = std::thread(ThreadFunc,this);//Asyncronous call and the function to be executed
            else
                this->ThreadFunc();
        }
        //Method for stopping the timer
        void Stop()
        {
            Alive = false ;
            Thread.join(); // This means that there will not be a concurrent execution on the one main thread
        }

        //Use this function when we create the Timer with an empty constructor and then we can set the values of variables ourselves
        void SetFunction(const std::function<void(void)> &f){funct = f;}

        bool IsAlive() const {return Alive;}

        void RepeatCount(const long r)
        {
            if(Alive)
                return;
            CallNumber = r ;
        }

        long GetLeftCount() const {return repeat_count;}

        long RepeatCount() const {return CallNumber;}

        void SetInterval(const unsigned long &i)
        {
            if(Alive)
                return;
            interval = std::chrono::milliseconds(i);
        }

        unsigned long Interval() const {return interval.count();}

        const std::function<void(void)> &Function() const {return funct;} // Look on the net


};

#endif // TIMER_H
