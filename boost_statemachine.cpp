#include <boost/statechart/transition.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <iostream>

struct lostMoney : boost::statechart::event<lostMoney> {};
struct gotMoney : boost::statechart::event<gotMoney> {};

struct HappyState;
struct SadState;
struct MoreHappyState;

class IState {
    public:
        virtual string getStateName() const = 0;
};

struct PersonSM : boost::statechart::state_machine<PersonSM, HappyState> {
    public:
    string getCurState() const
    {
        return string("CurrentState ------> ") + state_cast< const IState & >().getStateName();
    }
};


 

struct HappyState : boost::statechart::simple_state<HappyState, PersonSM>, public IState
{
    HappyState() { std::cout << "In Happy State..." << std::endl; }
    string getStateName() const
    {
        return string("HappyState");
    }

    typedef boost::mpl::list<boost::statechart::custom_reaction<lostMoney>,
                             boost::statechart::transition<gotMoney, MoreHappyState>> reactions;
    boost::statechart::result react(const lostMoney &lm)
    {
        std::cout << "I receive lost money event" << std::endl;
        return transit<SadState>();
    }

    ~HappyState() { std::cout << "In Hadppy State DESTRUCTION ..." << std::endl; }
};

struct MoreHappyState : boost::statechart::simple_state<MoreHappyState, PersonSM>, public IState
{
    MoreHappyState() { std::cout << "In More Happy State..." << std::endl; }
    ~MoreHappyState() { std::cout << "In More Happy State DESTRUCTION ..." << std::endl; }
    string getStateName() const
    {
        return string("MoreHappyState");
    }
};

struct SadState : boost::statechart::simple_state<SadState, PersonSM>, public IState
{
    SadState() { std::cout << "In Sad State..." << std::endl; }
    string getStateName() const
    {
        return string("SadState");
    }
    //typedef boost::statechart::transition<gotMoney, HappyState> reactions;
    typedef boost::mpl::list<boost::statechart::custom_reaction<lostMoney>,
                             boost::statechart::transition<gotMoney, HappyState>> reactions;
    boost::statechart::result react(const lostMoney &lm)
    {
        std::cout << "I receive lost money event in 2 lost" << std::endl;
        //return forward_event();
        return discard_event();
    }

    ~SadState() { std::cout << "In Sad State DESTRUCTION ..." << std::endl; }
};


/**
    rich  
    more rich
    best rich

    stateA
        stateB
        sateC

    stateD
        stateE
        stateF

*/
struct gotA : boost::statechart::event<gotA> {};
struct gotB : boost::statechart::event<gotB> {};
struct gotC : boost::statechart::event<gotC> {};
struct gotD : boost::statechart::event<gotD> {};
struct gotE : boost::statechart::event<gotE> {};
struct gotF : boost::statechart::event<gotF> {};

struct stateA;
struct stateB;
struct stateC;
struct stateF;

struct CharSM : boost::statechart::state_machine<CharSM, stateA> {
    public:
    string getCurState() const
    {
        return string("CurrentState ------> ") + state_cast< const IState & >().getStateName();
    }
};

struct stateA : boost::statechart::simple_state<stateA, CharSM, stateB>, public IState
{
    stateA() { std::cout << "In A..." << std::endl; }
    string getStateName() const
    {
        return string("stateA");
    }

    // forword _ event 这段代码非常神奇
    /*
        在C收到C的事件的时候，会forword； 如果A 可以处理 C， 和 A 不能处理C事件， 结果非常不一样！！！！！！

        CurrentState ------> stateC
        I receive C forword in C
        I receive C in A goto B
        In C DESTRUCTION ...
        In A DESTRUCTION ...
        In A...
        In B...
        CurrentState ------> stateB


        CurrentState ------> stateC
        I receive C forword in C
        CurrentState ------> stateC

        如果A处理C，只是forward
        CurrentState ------> stateC
        I receive C forword in C
        I receive C in A forward again
        CurrentState ------> stateC


        // A 处理C，但是 discard
        CurrentState ------> stateC
        I receive C forword in C
        I receive C in A discard
        CurrentState ------> stateC

        // A 处理C，只是 转到自己的状态, A的默认状态是B，相当于转到B
        CurrentState ------> stateC
        I receive C forword in C
        I receive C in A goto A
        In C DESTRUCTION ...
        In A DESTRUCTION ...
        In A...
        In B...
        CurrentState ------> stateB

    */
    typedef boost::mpl::list<
                            boost::statechart::custom_reaction<gotC>,
                            boost::statechart::custom_reaction<gotE>                            
                             > reactions;
    boost::statechart::result react(const gotC &lm)
    {
        //std::cout << "I receive C in A goto B" << std::endl;
        //return transit<stateB>();

        //std::cout << "I receive C in A forward again" << std::endl;
        //return forward_event();

        //std::cout << "I receive C in A discard" << std::endl;
        //return discard_event();

        std::cout << "I receive C in A goto A" << std::endl;
        return transit<stateA>();
    }
    


    boost::statechart::result react(const gotE &lm)
    {
        std::cout << "I receive E in A postevent to F" << std::endl;
        post_event(lm);
        return transit<stateF>();
    }

    ~stateA() { std::cout << "In A DESTRUCTION ..." << std::endl; }
};

struct stateB : boost::statechart::simple_state<stateB, stateA>, public IState
{
    stateB() { std::cout << "In B..." << std::endl; }
    string getStateName() const
    {
        return string("stateB");
    }

    typedef boost::mpl::list<boost::statechart::custom_reaction<gotB>,
                            boost::statechart::custom_reaction<gotC>,
                            boost::statechart::custom_reaction<gotE>,

                            boost::statechart::custom_reaction<gotD>,
                            // 不在reaction列表里的，进入 gotF； 在列表里，但是没有 react函数的，进入 react event_base函数
                        
                            boost::statechart::transition< boost::statechart::event_base, stateF >
                               > reactions;

    boost::statechart::result react(const boost::statechart::event_base&) {
        std::cout << "eventbase discard" << std::endl;
        return discard_event();
    }

    boost::statechart::result react(const gotB &lm)
    {
        std::cout << "I receive B in B discard" << std::endl;
        //return transit<stateB>();
        return discard_event();
    }

    boost::statechart::result react(const gotC &lm)
    {
        std::cout << "I receive C in B go to C" << std::endl;
        return transit<stateC>();
    }

    boost::statechart::result react(const gotE &lm)
    {
        std::cout << "I receive E in B go to forword" << std::endl;
        return forward_event();
    }

    boost::statechart::result react(const gotF &lm)
    {
        std::cout << "I receive F in B discard....." << std::endl;
        return discard_event();
    }

    ~stateB() { std::cout << "In B DESTRUCTION ..." << std::endl; }
};

struct stateC : boost::statechart::simple_state<stateC, stateA>, public IState
{
    stateC() { std::cout << "In C..." << std::endl; }
    string getStateName() const
    {
        return string("stateC");
    }

    typedef boost::mpl::list<boost::statechart::custom_reaction<gotB>,
                            boost::statechart::custom_reaction<gotC>
                             /*boost::statechart::transition<gotC, MoreHappyState> */> reactions;
    boost::statechart::result react(const gotB &lm)
    {
        std::cout << "I receive B in C" << std::endl;
        return transit<stateB>();
    }

    boost::statechart::result react(const gotC &lm)
    {
        std::cout << "I receive C forword in C" << std::endl;
        return forward_event();
    }


    boost::statechart::result react(const gotE &lm)
    {
        std::cout << "I receive C forword in D" << std::endl;
        return forward_event();
    }

    ~stateC() { std::cout << "In C DESTRUCTION ..." << std::endl; }
};

struct stateF : boost::statechart::simple_state<stateF, stateA>, public IState
{
    stateF() { std::cout << "In F..." << std::endl; }
    string getStateName() const
    {
        return string("stateF");
    }

    typedef boost::mpl::list<boost::statechart::custom_reaction<gotB>,
                            boost::statechart::custom_reaction<gotE>
                             /*boost::statechart::transition<gotC, MoreHappyState> */> reactions;
    boost::statechart::result react(const gotB &lm)
    {
        std::cout << "I receive B in F" << std::endl;
        return transit<stateB>();
    }

    boost::statechart::result react(const gotE &lm)
    {
        std::cout << "I receive E  in F goto C" << std::endl;
        return transit<stateC>();
    }

    ~stateF() { std::cout << "In F DESTRUCTION ..." << std::endl; }
};



void testboostsc(){
    CharSM c;
    c.initiate();
    std::cout << c.getCurState() << std::endl;
    std::cout << "will process D" << std::endl;
    c.process_event(gotD());
    std::cout << c.getCurState() << std::endl;
    std::cout << "will process F" << std::endl;
    c.process_event(gotF());
    std::cout << c.getCurState() << std::endl;

    std::cout << "will process B" << std::endl;
    c.process_event(gotB());
    std::cout << c.getCurState() << std::endl;
    
    std::cout << "will process E" << std::endl;
    c.process_event(gotE());
    std::cout << c.getCurState() << std::endl;
    
    return;
    std::cout << c.getCurState() << std::endl;
    
    c.process_event(gotB());
    std::cout << c.getCurState() << std::endl;
    c.process_event(gotC());
    std::cout << c.getCurState() << std::endl;
    c.process_event(gotC());
    std::cout << c.getCurState() << std::endl;
    c.process_event(gotB());
    std::cout << c.getCurState() << std::endl;
    c.process_event(gotC());
    std::cout << c.getCurState() << std::endl;
    

    return;
    PersonSM sm;
    sm.initiate();
    std::cout << sm.getCurState()<<std::endl;
    sm.process_event(lostMoney());
    std::cout << sm.getCurState()<<std::endl;
    sm.process_event(lostMoney());
    std::cout << sm.getCurState()<<std::endl;
    sm.process_event(lostMoney());
    std::cout << sm.getCurState()<<std::endl;
    
    sm.process_event(gotMoney());
    std::cout  << sm.getCurState()<<std::endl;

    sm.process_event(gotMoney());    
    std::cout  << sm.getCurState() <<std::endl;

    sm.process_event(gotMoney());    
    std::cout  << sm.getCurState() <<std::endl;
    sm.process_event(lostMoney());    
    std::cout  << sm.getCurState() <<std::endl;
}
