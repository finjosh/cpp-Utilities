#include "Utils/TerminatingFunction.hpp"

void TerminatingFunction::Data::setRunning()
{
    m_state = State::Running;
}

void TerminatingFunction::Data::setFinished()
{
    m_state = State::Finished;
}

TerminatingFunction::State TerminatingFunction::Data::getState() const
{
    return m_state;
}

float TerminatingFunction::Data::getDeltaTime() const
{
    return m_deltaTime;
}

float TerminatingFunction::Data::getTotalTime() const
{
    return m_totalTime;
}

bool TerminatingFunction::Data::isForceStop() const
{
    return m_isForceStop;
}

bool TerminatingFunction::Data::isStopRequested() const
{
    return m_requestStop;
}

bool TerminatingFunction::_tFunc::operator== (const _tFunc& tFunc) const
{
    return m_func.getTypeid() == tFunc.m_func.getTypeid();
}

bool TerminatingFunction::_tFunc::operator== (const std::string& id) const
{
    return m_func.getTypeid() == id;
}

bool TerminatingFunction::_tFunc::operator< (const _tFunc& tFunc) const
{
    return m_func.getTypeid() < tFunc.m_func.getTypeid();
}

bool TerminatingFunction::_tFunc::operator< (const std::string& id) const
{
    return m_func.getTypeid() < id;
}

std::string TerminatingFunction::_tFunc::getShortenedID(const std::string& typeID)
{
    return typeID.substr(typeID.find("setFunction")+11);
}

std::list<std::string> TerminatingFunction::_tFunc::toString()
{
    return {getShortenedID(m_func.getTypeid()), std::to_string(m_totalTime), std::to_string(m_maxTime)};
}

std::list<TerminatingFunction::_tFunc> TerminatingFunction::m_terminatingFunctions;
std::mutex TerminatingFunction::m_lock;

void TerminatingFunction::UpdateFunctions(float deltaTime)
{
    m_lock.lock();
    auto function = TerminatingFunction::m_terminatingFunctions.begin();

    Data data;

    while (function != TerminatingFunction::m_terminatingFunctions.end())
    {
        function->m_totalTime += deltaTime;
        bool finished = (function->m_maxTime <= function->m_totalTime);
        
        //* setting data up
        data.m_deltaTime = deltaTime;
        data.m_isForceStop = finished;
        data.m_requestStop = function->m_requestStop;
        data.m_totalTime = function->m_totalTime;
        data.setFinished(); // default to being finished
        m_lock.unlock();
        function->m_func.invoke(&data); // calling the function
        m_lock.lock();

        if (data.getState() == TerminatingFunction::State::Finished || finished)
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

std::string TerminatingFunction::Add(funcHelper::funcDynamic<Data*> function, float maxTime)
{ 
    m_lock.lock();
    if (function.isValid())
    {
        TerminatingFunction::m_terminatingFunctions.emplace_back(function, maxTime); 
        
        m_lock.unlock();
        return function.getTypeid();
    }
    m_lock.unlock();
    return "";
}

void TerminatingFunction::clear()
{ 
    m_lock.lock(); 
    for (auto function: m_terminatingFunctions)
    {
        function.m_maxTime = 0.f;
    }
    UpdateFunctions(0);
    m_lock.unlock(); 
}

void TerminatingFunction::remove(const std::string& functionTypeid)
{
    m_lock.lock();
    auto iter = std::find(m_terminatingFunctions.begin(), m_terminatingFunctions.end(), functionTypeid);
    if (iter != m_terminatingFunctions.end())
        iter->m_maxTime = 0.f;
    m_lock.unlock();
}

void TerminatingFunction::forceRemove(const std::string& functionTypeid)
{
    m_lock.lock();
    std::remove(m_terminatingFunctions.begin(), m_terminatingFunctions.end(), functionTypeid);
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

bool TerminatingFunction::requestStop(const std::string& functionTypeid)
{
    m_lock.lock();
    auto iter = std::find(m_terminatingFunctions.begin(), m_terminatingFunctions.end(), functionTypeid);
    if (iter != m_terminatingFunctions.end())
    {
        iter->m_requestStop = true;
        m_lock.unlock();
        return true;
    }
    m_lock.unlock();
    return false;
}
