#include "Utils/TerminatingFunction.hpp"

std::list<TerminatingFunction::_tFunc> TerminatingFunction::terminatingFunctions;

void TerminatingFunction::UpdateFunctions(float deltaTime)
{
    auto function = TerminatingFunction::terminatingFunctions.begin();
    while (function != TerminatingFunction::terminatingFunctions.end())
    {
        function->totalTime += deltaTime;
        data rtnData(deltaTime, function->totalTime);
        function->func.invoke(&rtnData);

        if (rtnData.state == TerminatingFunction::State::Finished)
        {
            auto temp = function;
            function++;
            TerminatingFunction::terminatingFunctions.erase(temp);
            if (TerminatingFunction::terminatingFunctions.size() == 0) return;
            continue;
        }
        function++;
    }
}

std::string TerminatingFunction::Add(funcHelper::funcDynamic<data*> function)
{ 
    if (function.valid())
    {
        TerminatingFunction::terminatingFunctions.push_back({function}); 
        
        return function.getTypeid();
    }
    return "";
}

void TerminatingFunction::clear()
{ TerminatingFunction::terminatingFunctions.clear(); }

void TerminatingFunction::remove(const std::string& functionTypeid)
{
    TerminatingFunction::terminatingFunctions.remove_if([functionTypeid](const _tFunc& func){ return functionTypeid == func.func.getTypeid(); });
    return;
}

std::list<std::pair<std::string, std::string>> TerminatingFunction::getStringData()
{
    std::list<std::pair<std::string, std::string>> rtn;

    for (auto func: terminatingFunctions)
    {
        rtn.push_back(func.toString());
    }

    return rtn;
}
