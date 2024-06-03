#include "Utils/TerminatingFunction.hpp"

std::list<TerminatingFunction::_tFunc> TerminatingFunction::m_terminatingFunctions;
std::mutex TerminatingFunction::m_lock;

void TerminatingFunction::UpdateFunctions(float deltaTime)
{
    m_lock.lock();
    auto function = TerminatingFunction::m_terminatingFunctions.begin();
    while (function != TerminatingFunction::m_terminatingFunctions.end())
    {
        function->totalTime += deltaTime;
        bool finished = (function->maxTime <= function->totalTime);
        data rtnData(deltaTime, function->totalTime, finished);
        function->func.invoke(&rtnData);

        if (rtnData.state == TerminatingFunction::State::Finished || finished)
        {
            auto temp = function;
            function++;
            TerminatingFunction::m_terminatingFunctions.erase(temp);
            if (TerminatingFunction::m_terminatingFunctions.size() == 0) break;
            continue;
        }
        function++;
    }
    m_lock.unlock();
}

std::string TerminatingFunction::Add(funcHelper::funcDynamic<data*> function, float maxTime)
{ 
    m_lock.lock();
    if (function.valid())
    {
        TerminatingFunction::m_terminatingFunctions.push_back({function, maxTime}); 
        
        m_lock.unlock();
        return function.getTypeid();
    }
    m_lock.unlock();
    return "";
}

void TerminatingFunction::clear()
{ 
    m_lock.lock(); 
    TerminatingFunction::m_terminatingFunctions.clear(); 
    m_lock.unlock(); 
}

void TerminatingFunction::remove(const std::string& functionTypeid)
{
    m_lock.lock();
    std::find_if(m_terminatingFunctions.begin(), m_terminatingFunctions.end(), [functionTypeid](const _tFunc& func){ return functionTypeid == func.func.getTypeid(); })->maxTime = 0.f;
    m_lock.unlock();
}

void TerminatingFunction::forceRemove(const std::string& functionTypeid)
{
    m_lock.lock();
    TerminatingFunction::m_terminatingFunctions.remove_if([functionTypeid](const _tFunc& func){ return functionTypeid == func.func.getTypeid(); });
    m_lock.unlock();
}

std::list<std::list<std::string>> TerminatingFunction::getStringData()
{
    m_lock.lock();
    std::list<std::list<std::string>> rtn;

    for (auto func: m_terminatingFunctions)
    {
        rtn.push_back(func.toString());
    }
    m_lock.unlock();

    return rtn;
}
