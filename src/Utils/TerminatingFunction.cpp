#include "include\Utils\TerminatingFunction.h"

std::unordered_multiset<funcHelper::func<TF::StateType>> TerminatingFunction::terminatingFunctions;
float TerminatingFunction::deltaTime = 0;

void TerminatingFunction::UpdateFunctions(float deltaTime)
{
    TerminatingFunction::deltaTime = deltaTime;
    auto function = TerminatingFunction::terminatingFunctions.begin();
    while (function != TerminatingFunction::terminatingFunctions.end())
    {
        if (!function->valid() || function->invoke() == StateType::Finished)
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

std::string TerminatingFunction::Add(funcHelper::func<StateType> function, bool replace)
{ 
    if (function.valid())
    {
        if (replace && TerminatingFunction::terminatingFunctions.contains(function))
        {
            TerminatingFunction::terminatingFunctions.erase(function);
            TerminatingFunction::terminatingFunctions.emplace(function); 
        }
        else
            TerminatingFunction::terminatingFunctions.emplace(function); 
        
        return function.getTypeid();
    }
    return "";
}

void TerminatingFunction::clear()
{ TerminatingFunction::terminatingFunctions.clear(); }

void TerminatingFunction::erase(const std::string& functionTypeid)
{
    std::erase_if(terminatingFunctions, [functionTypeid](const funcHelper::func<void>& func){ return functionTypeid == func.getTypeid(); });
    return;
}

float TerminatingFunction::getDeltaTime()
{ return TerminatingFunction::deltaTime; }
